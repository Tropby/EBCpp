#pragma once

#include <iostream>
#include <map>
#include <sstream>

#include <openssl/evp.h>
#include <openssl/sha.h>

#include "../EBEvent.hpp"
#include "../EBObject.hpp"
#include "../socket/tcp/EBTcpSocket.hpp"

#include "../http/EBHttpRequest.hpp"

namespace EBCpp
{

/**
 * @brief Handles a Websocket request
 * @todo Kill connection on close!
 */
class EBWebSocket : public EBObject<EBWebSocket>
{
public:
    /**
     * @brief Construct a new EBWebSocket object
     */
    EBWebSocket(EBObjectPointer<EBHttpRequest> request) :
        tcpSocket(request->getSocket()), readState(RS_INIT), payloadSize(0), pos(0)
    {
        request->setSocket(nullptr);
        EB_EMIT(request->finished);

        tcpSocket->readReady.connect(this, &EBWebSocket::tcpReadReady);
        tcpSocket->disconnected.connect(this, &EBWebSocket::tcpDisconnected);

        std::string key = request->getRequestHeader().getValue("Sec-WebSocket-Key");
        key += "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
        unsigned char c[20];
        SHA1(reinterpret_cast<const unsigned char*>(key.c_str()), key.size(), c);
        char output[64];
        base64encode(c, 20, output);

        key = output;

        tcpSocket->write("HTTP/1.1 101 Switching Protocols\r\n");
        tcpSocket->write("Upgrade: websocket\r\n");
        tcpSocket->write("Connection: Upgrade\r\n");
        tcpSocket->write("Sec-WebSocket-Accept: " + key + "\r\n");
        tcpSocket->write("Sec-WebSocket-Protocol: chat\r\n");
        tcpSocket->write("\r\n");
    }

    /**
     * @brief Destroy the EBWebSocket object
     * 
     */
    ~EBWebSocket()
    {
        tcpSocket = nullptr;
    }

    /**
     * @brief Encodes a input data to its Base64 representation usign OpenSSL
     * @todo Move this method to EBUtils for OpenSSL
     *
     * @param input data to encode
     * @param length legnth of the string to encode
     * @param output buffer for the encoded output
     * @return char* the output pointer
     */
    char* base64encode(unsigned char* input, int length, char* output)
    {
        const auto pl = 4 * ((length + 2) / 3);
        const auto ol = EVP_EncodeBlock(reinterpret_cast<unsigned char*>(output), input, length);
        if (pl != ol)
        {
            std::cerr << "Whoops, encode predicted " << pl << " but we got " << ol << "\n";
        }
        return output;
    }

    /**
     * @brief EB_SIGNAL readReady
     *
     * Signal is emitted if the request receive has been finished
     */
    EB_SIGNAL(readReady);

    EB_SIGNAL(disconnected);

    EB_SIGNAL_WITH_ARGS(textFrameReceived, std::string);

    /**
     * @brief EB_SIGNAL binaryFrameReceived
     *
     */
    EB_SIGNAL_WITH_ARGS(binaryFrameReceived, std::vector<uint8_t>);

    /**
     * @brief EB_SIGNAL finished
     *
     * This signal will be emitted if the request is finished and the
     * reply is send to the client.
     */
    EB_SIGNAL(finished);

    void sendTextFrame(std::string& text) 
    {
        send(OC_TEXT_FRAME, text.c_str(), text.size(), false);
    }

    void sendBinaryFrame(std::vector<uint8_t>& data) 
    {
        send(OC_BINARY_FRAME, reinterpret_cast<char*>(data.data()), data.size(), false);
    }

private:
    EBObjectPointer<EBTcpSocket> tcpSocket;

    EB_SLOT(tcpDisconnected)
    {
        // Disconnect from the tcp socket
        tcpSocket->readReady.disconnect(this, &EBWebSocket::tcpReadReady);
        tcpSocket->disconnected.disconnect(this, &EBWebSocket::tcpDisconnected);

        // emit that the websocket connection has endet
        EB_EMIT(disconnected);
    }

    enum READ_STATE
    {
        RS_INIT,
        RS_PAYLOAD,
        RS_PAYLOAD16,
        RS_PAYLOAD64,
        RS_MASK,
        RS_DATA
    } readState;
    uint64_t payloadSize;

    enum OPCODE
    {
        OC_CONTINUATION_FRAME = 0x00,
        OC_TEXT_FRAME = 0x01,
        OC_BINARY_FRAME = 0x02,
        OC_CONNECTION_CLOSE = 0x08,
        OC_PING = 0x09,
        OC_PONG = 0x0A
    } opcode;
    uint64_t pos;

    /// TODO: data must be created and destroyed by payload + 1
    unsigned char data[1024];
    unsigned char mask[4];
    bool isMask;

    bool fin;

    void send(OPCODE opcode, const char* body, uint64_t size, bool mask = false) 
    {
        char c[1024];
        int pos = 0;
        c[pos++] = 0x80 | opcode;

        if (size <= 125)
        {
            c[pos++] = size;
        }
        else if (size <= 65535)
        {
            c[pos++] = 126;
            c[pos++] = (size >> 8) & 0xFF;
            c[pos++] = (size >> 0) & 0xFF;
        }
        else
        {
            c[2] = 127;
            c[pos++] = (size >> 56) & 0xFF;
            c[pos++] = (size >> 48) & 0xFF;
            c[pos++] = (size >> 40) & 0xFF;
            c[pos++] = (size >> 32) & 0xFF;
            c[pos++] = (size >> 24) & 0xFF;
            c[pos++] = (size >> 16) & 0xFF;
            c[pos++] = (size >> 8) & 0xFF;
            c[pos++] = (size >> 0) & 0xFF;
        }

        // MASK (if send with mask, currently not supported)
        /// TODO: Add random mask to the message
        if (mask)
        {
            c[pos++] = 0x00;
            c[pos++] = 0x00;
            c[pos++] = 0x00;
            c[pos++] = 0x00;
        }

        for (int i = 0; i < size; i++)
        {
            c[pos++] = body[i];
        }

        tcpSocket->write(c, pos);
    }

    EB_SLOT(tcpReadReady)
    {
        EB_PROFILE_FUNC();

        unsigned char c[1024];
        int nBytes = tcpSocket->read(reinterpret_cast<char*>(c), 1024);

        for (int i = 0; i < nBytes; i++)
        {
            // current byte
            uint8_t d = c[i];

            switch (readState)
            {
            // Read the first byte and interprete the opcode
            case RS_INIT:
                opcode = static_cast<OPCODE>(d & 0x0F);
                fin = ((d & 0x80) == 0x80);
                readState = RS_PAYLOAD;
                break;

            // Read the payload size
            case RS_PAYLOAD: {

                isMask = ((d & 0x80) == 0x80);

                switch (d & 0x7F)
                {
                case 126:
                    readState = RS_PAYLOAD16;
                    break;

                case 127:
                    readState = RS_PAYLOAD64;
                    break;

                default:
                    payloadSize = d & 0x7F;
                    pos = 0;
                    readState = RS_MASK;

                    if (payloadSize == 0 && !isMask)
                        checkMessage();
                    break;
                }

                break;
            }

            // Read two more bytes for 16 bit payload size
            case RS_PAYLOAD16:
                payloadSize <<= 8;
                payloadSize |= d;
                pos++;
                if (pos == 2)
                {
                    pos = 0;
                    readState = RS_MASK;

                    if (payloadSize == 0 && !isMask)
                        checkMessage();
                }
                break;

            // Read 8 more bytes for 64 bit payload size
            case RS_PAYLOAD64:
                payloadSize <<= 8;
                payloadSize |= d;
                pos++;
                if (pos == 8)
                {
                    pos = 0;
                    readState = RS_MASK;

                    if (payloadSize == 0 && !isMask)
                        checkMessage();
                }
                break;

            // Read the mask
            case RS_MASK:
                mask[pos++] = d;
                if (pos == 4)
                {
                    pos = 0;
                    readState = RS_DATA;

                    if (payloadSize == 0)
                        checkMessage();
                }
                break;

            // Read the actual payload
            case RS_DATA:
                if (isMask)
                    data[pos++] = d ^ mask[pos % 4];
                else
                    data[pos++] = d;

                checkMessage();
                break;
            }
        }
    }

    void checkMessage()
    {
        if (pos >= payloadSize)
        {
            switch (opcode)
            {
            case OC_TEXT_FRAME:
            {
                data[pos] = 0x00;
                std::string dataStr(reinterpret_cast<char*>(data));
                EB_EMIT_WITH_ARGS(textFrameReceived, dataStr);
                break;
            }

            case OC_BINARY_FRAME:
            {
                std::vector<uint8_t> binaryData = std::vector<uint8_t>(data, data + payloadSize);
                EB_EMIT_WITH_ARGS(binaryFrameReceived, binaryData);
                break;
            }

            case OC_CONNECTION_CLOSE:
            {
                send(OC_CONNECTION_CLOSE, reinterpret_cast<char*>(data), pos, false);
                tcpSocket->close();
                break;
            }

            default:
                EB_LOG_WARNING("Unhandled Websocket OPCODE received [" << std::hex << opcode << "]!");
                break;
            }
            pos = 0;
            readState = RS_INIT;
        }
    }
};

} // namespace EBCpp
