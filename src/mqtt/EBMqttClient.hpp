/*
* EBCpp Project
* https://github.com/Tropby/EBCpp
* 
* ---
* 
* MIT License
* 
* Copyright (c) 2024 Carsten (Tropby)
* 
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
* 
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
* 
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*
*  Created on: 2022-06-17
*      Author: Carsten (Tropby)
*/

#pragma once

#include <mutex>

#include "../EBObject.hpp"
#include "../EBUrl.hpp"
#include "../socket/tcp/EBTcpSocket.hpp"

#include "EBMqtt.hpp"

namespace EBCpp
{

/**
 * @brief Mqtt Client class
 * 
 * @tparam socket The socket type that is used
 */
template <class socket = EBTcpSocket>
class EBMqttClient : public EBObject<EBMqttClient<> >
{
public:
    EBMqttClient()
    {
        _receivedCommand.state = STATE::PACKAGE_TYPE;

        _tcpSocket.readReady.connect(this, &EBMqttClient::tcpReadReady);
        _tcpSocket.connected.connect(this, &EBMqttClient::tcpConnected);
        _tcpSocket.disconnected.connect(this, &EBMqttClient::tcpDisconnected);
    }

    /**
     * @brief Set the Url to connect to. Format: "tcp://x.x.x.x:8334/"
     * 
     * @param url 
     */
    void setUrl(const EBUrl& url)
    {
        this->_url = url;
    }

    /**
     * @brief Set the Username
     * 
     * @param username 
     */
    void setUsername(const EBString& username)
    {
        this->_username = username;
    }

    /**
     * @brief Set the Password
     * 
     * @param password 
     */
    void setPassword(const EBString& password)
    {
        this->_password = password;
    }

    /**
     * @brief Set the Client Id
     * 
     * @param clientId 
     */
    void setClientId(const EBString& clientId)
    {
        this->_clientId = clientId;
    }

    /**
     * @brief Connect to the broker
     * 
     * @return true if connection is opened
     * @return false if there was a issu creating the conneciton
     */
    bool connectToHost()
    {
        _tcpSocket.setFileName(_url.toString());
        return _tcpSocket.open(EBTcpSocket::READ_WRITE);
    }

    /**
     * @brief Subscribes to a topic on the connected broker
     * 
     * @param topic The topic to subscribe to
     * @param qos The quality of service for this subscribtion
     * @return uint16_t Packet Identifier for the SUBSCRIBE package
     */
    uint16_t subscribe(const EBString& topic, uint8_t qos = 1)
    {
        return sendSubscribe(topic, qos);
    }

    /**
     * @brief Publish data to a topic
     * 
     * @param topic The topic to publish the data
     * @param qos 
     * @param retain 
     * @param payload 
     */
    uint16_t publish(const EBString& topic, const EBString& payload, EBMqtt::QOS qos = EBMqtt::QOS::QOS0, EBMqtt::RETAIN retain = EBMqtt::RETAIN::NO)
    {
        return sendPublish(topic, 0, qos, retain, payload.dataPtr(), payload.length());
    }

    uint16_t publish(const EBString& topic, const char * data, uint32_t size, EBMqtt::QOS qos = EBMqtt::QOS::QOS0, EBMqtt::RETAIN retain = EBMqtt::RETAIN::NO)
    {
        return sendPublish(topic, 0, qos, retain, data, size);
    }

    uint16_t publish(const EBString& topic, EBMqtt::QOS qos = EBMqtt::QOS::QOS0, EBMqtt::RETAIN retain = EBMqtt::RETAIN::NO)
    {
        return sendPublish(topic, 0, qos, retain, nullptr, 0);
    }

    /**
     * @brief This signal is emitted if the tcp connection is established
     * 
     */
    EB_SIGNAL(connected);

    /**
     * @brief This signal is emitted if the tcp connection is closed
     * 
     */
    EB_SIGNAL(disconnected);

    /**
     * @brief This signal is emitted if a CONNACK is received. 
     *        This happens after a connection is established with connectToHost().
     * 
     * @param reasonCode SUCCESS or an error code
     */
    EB_SIGNAL_WITH_ARGS(connack, EBMqtt::CONNACK_REASON_CODE);

    /**
     * @brief This signal is emitted if the DISCONNECT package is received
     * 
     * @param reasonCode SUCCESS or an error code
     */
    EB_SIGNAL_WITH_ARGS(disconnect, EBMqtt::DISCONNECT_REASON_CODE);

    /**
     * @brief The signal ist emitted if a SUBACK is received
     * 
     * @param packateIdentifier The packet identifier
     */
    EB_SIGNAL_WITH_ARGS(suback, uint16_t);

    EB_SIGNAL_WITH_ARGS(puback, uint16_t);

private:
    socket _tcpSocket;
    EBUrl _url;
    EBString _username;
    EBString _password;
    EBString _clientId = "EBMqttClient";

    EB_SLOT(tcpDisconnected)
    {
        EB_EMIT(disconnected);
    }

    /**
     * @brief This slot is called if the tcp connection is established
     * 
     */
    EB_SLOT(tcpConnected)
    {
        EB_EMIT(connected);
        sendConnect();
    }

    /**
     * @brief This slot is called if new data is available at the tcpsocket.
     * 
     */
    EB_SLOT(tcpReadReady)
    {
        uint8_t buffer[1024];

        while( int len = _tcpSocket.read((char*)buffer, 1024) )
        {
            for (int i = 0; i < len; i++ )
            {
                parseData(buffer[i]);
            }
        }
    }

    /**
     * @brief Parses the data
     * 
     * @param data The next byte to parse
     */
    void parseData(uint8_t data)
    {
        switch( _receivedCommand.state )
        {
            case STATE::PACKAGE_TYPE:
                _receivedCommand.packageType = (EBMqtt::PACKET_TYPE)((data >> 4) & 0x0F);
                _receivedCommand.packageTypeFlags = data & 0x0F;
                _receivedCommand.length = 0;
                _receivedCommand.multiplier = 1;
                _receivedCommand.dataPos = 0;
                _receivedCommand.state = STATE::LENGTH;
                break;
            case STATE::LENGTH:
                _receivedCommand.length += (data & 0x7F) * _receivedCommand.multiplier;
                _receivedCommand.multiplier *= 0x80;

                if( !(data & 0x80) )
                {
                    _receivedCommand.state = STATE::DATA;
                }
                break;
            case STATE::DATA:
                _receivedCommand.data[_receivedCommand.dataPos++] = data;
                if( _receivedCommand.length == _receivedCommand.dataPos )
                {
                    dumpCommand();
                    interpreteCommand();
                    _receivedCommand.state = STATE::PACKAGE_TYPE;
                }
                break;
            default:
                throw EBException("Error parsing MQTT data!");
                break;
        }
    }
    
    void interpreteCommand()
    {
        switch( _receivedCommand.packageType )
        {
            case EBMqtt::PACKET_TYPE::PUBLISH:
                interpretePublish();
                break;

            case EBMqtt::PACKET_TYPE::PUBACK:
                interpretePuback();
                break;

            case EBMqtt::PACKET_TYPE::CONNACK:
                interpreteConnack();
                break;

            case EBMqtt::PACKET_TYPE::SUBACK:
                interpreteSuback();
                break;

            case EBMqtt::PACKET_TYPE::DISCONNECT:
                interpreteDisconnect();
                break;

            default:
                EB_LOG_CRITICAL("Unknwon Package Type: " << (int)_receivedCommand.packageType);
                exit(1);
                break;
        }
    }

    void interpretePuback()
    {
        uint16_t id = (((uint16_t)_receivedCommand.data[0]) << 8) & 0xFF00;
        id |= ((uint16_t)_receivedCommand.data[1]) & 0xFF;

        resetPacketIdentifier(id);

        auto prc = (EBMqtt::PUBACK_REASON_CODE)_receivedCommand.data[2];

        // TODO: Implement Properties

        EB_EMIT_WITH_ARGS(puback, id);
    }

    void interpreteSuback()
    {
        uint16_t id = (((uint16_t)_receivedCommand.data[0]) << 8) & 0xFF00;
        id |= ((uint16_t)_receivedCommand.data[1]) & 0xFF;

        resetPacketIdentifier(id);

        // TODO: Implement Properties

        EB_EMIT_WITH_ARGS(suback, id);
    }

    void interpreteDisconnect()
    {
        auto reasonCode = EBMqtt::DISCONNECT_REASON_CODE::NORMAL_DISCONNECTION;
        if( _receivedCommand.length >= 1 )
        {
            reasonCode = (EBMqtt::DISCONNECT_REASON_CODE)_receivedCommand.data[0];
        }

        // TODO: Implement Properties

        EB_EMIT_WITH_ARGS(disconnect, reasonCode);
    }

    void interpreteConnack()
    {
        uint32_t pos = 0;
        uint8_t flags = _receivedCommand.data[pos++];
        auto reasonCode = (EBMqtt::CONNACK_REASON_CODE)_receivedCommand.data[pos++];

        // TODO: Implement Properties

        EB_EMIT_WITH_ARGS(connack, reasonCode);
    }

    void interpretePublish()
    {
        EB_LOG_DEBUG("INTERPRETE PUBLISH!");

        uint32_t pos = 0;

        EBString topic;
        topic = EBMqtt::getString(&(_receivedCommand.data[pos]), sizeof(_receivedCommand.data) - pos - 1, &pos);

        EB_LOG_DEBUG("    TOPIC: " << topic);

        int ident = 0;
        if( _receivedCommand.packageTypeFlags & 0x06 )
        {
            ident = _receivedCommand.data[pos++];
            ident <<= 8;
            ident += _receivedCommand.data[pos++];

            EB_LOG_DEBUG("    IDENT: " << EBUtils::intToHex( ident ));
        }

        uint32_t propertyLength;
        pos += EBMqtt::VBIToInt(_receivedCommand.data, sizeof(_receivedCommand.data) - pos - 1, &propertyLength);
        EB_LOG_DEBUG("    PROPERTY LENGTH: " << propertyLength);
        if( propertyLength > 0 )
        {
            for (uint32_t i = 0; i < propertyLength; i++ )
            {
                uint8_t propertyId = _receivedCommand.data[pos++];
                EB_LOG_DEBUG("         PROPERTY ID: " << EBUtils::charToHex(propertyId));
            }
        }

        int payloadSize = _receivedCommand.length - pos;
        EBString payload((char*)&(_receivedCommand.data[pos]), payloadSize);
        EB_LOG_DEBUG("    PAYLOAD ["<< payloadSize <<"]: " << payload);
    }

    void dumpCommand()
    {
        EB_LOG_DEBUG(
            " Package Type: " << EBUtils::charToHex( (uint8_t)_receivedCommand.packageType ) <<
            " Package Type Flags: " << EBUtils::charToHex( _receivedCommand.packageTypeFlags ) <<
            " Length: " << EBUtils::intToHex( _receivedCommand.length ) <<
            " Payload: " << EBUtils::binToHex(_receivedCommand.data, _receivedCommand.length)
        );
    }

    void sendConnect()
    {
        uint8_t buffer[1024];

        //Variable Header
        // Protocol Name
        int pos = 0;
        buffer[pos++] = 0;
        buffer[pos++] = 4;
        buffer[pos++] = 'M';
        buffer[pos++] = 'Q';
        buffer[pos++] = 'T';
        buffer[pos++] = 'T';

        // Protocol Version
        buffer[pos++] = 5;

        // Connect Flags
        buffer[pos++] =
            (uint8_t)EBMqtt::CONNECT_FLAGS::CLEAN_START |
            (uint8_t)EBMqtt::CONNECT_FLAGS::USERNAME |
            (uint8_t)EBMqtt::CONNECT_FLAGS::PASSWORD;

        // Keep Alive 60 sec
        buffer[pos++] = 0;
        buffer[pos++] = 60;

        // Properties
        buffer[pos++] = 5;
        buffer[pos++] = 17;
        buffer[pos++] = 0;
        buffer[pos++] = 0;
        buffer[pos++] = 0;
        buffer[pos++] = 10;

        pos += EBMqtt::string(this->_clientId, &(buffer[pos]), sizeof(buffer) - pos - 1);
        pos += EBMqtt::string(this->_username, &(buffer[pos]), sizeof(buffer) - pos - 1);
        pos += EBMqtt::string(this->_password, &(buffer[pos]), sizeof(buffer) - pos - 1);

        // Send Message
        send(EBMqtt::PACKET_TYPE::CONNECT, 0, buffer, pos);
    }

    uint16_t sendPublish(const EBString& topic, uint8_t resend, EBMqtt::QOS qos, EBMqtt::RETAIN retain, const char * payload, uint32_t payloadSize)
    {
        int pos = 0;
        uint8_t buffer[1024];
        uint16_t result = 0;

        pos += EBMqtt::string(topic, &(buffer[pos]), sizeof(buffer) - pos - 1);

        if( (uint8_t)qos > 0 )
        {
            uint16_t currentPackageIdentifier = getFreePacketIdentifier();
            result = currentPackageIdentifier;
            if( currentPackageIdentifier == 0 )
            {
                throw EBException("There is no free packet identifier!");
            }
            buffer[pos++] = (currentPackageIdentifier >> 8) & 0xFF;
            buffer[pos++] = (currentPackageIdentifier >> 0) & 0xFF;
        }

        buffer[pos++] = 0;

        // Set Flags
        int flags = 0x00;
        if( retain == EBMqtt::RETAIN::YES )
        {
            flags |=(1 << 0);
        }

        if( auto iqos = (uint8_t)qos )
        {
            iqos &= 0x03;
            flags |= (iqos << 1);

            if( resend )
            {
                flags |= (1 << 3);
            }
        }

        // Copy buffer
        for (int i = 0; i < payloadSize; i++ )
        {
            buffer[pos] = payload[i];
            pos++;
        }

        send(EBMqtt::PACKET_TYPE::PUBLISH, flags, buffer, pos);

        return result;
    }

    uint16_t sendSubscribe(const EBString& topic, uint8_t options)
    {
        int pos = 0;
        uint8_t buffer[1024];
        uint16_t currentPackageIdentifier = getFreePacketIdentifier();

        if( currentPackageIdentifier == 0 )
        {
            throw EBException("There is no free packet identifier!");
        }

        buffer[pos++] = (currentPackageIdentifier >> 8) & 0xFF;
        buffer[pos++] = (currentPackageIdentifier >> 0) & 0xFF;
        buffer[pos++] = 0;

        pos += EBMqtt::string(topic, &(buffer[pos]), sizeof(buffer) - pos - 2);
        buffer[pos++] = options;

        send(EBMqtt::PACKET_TYPE::SUBSCRIBE, 0x02, buffer, pos);

        return currentPackageIdentifier;
    }

    void send(EBMqtt::PACKET_TYPE type, int packageFlags, uint8_t * buffer, uint32_t size)
    {
        const std::scoped_lock lock{mutex};

        uint8_t fixedHeader[8];
        fixedHeader[0] = (uint8_t)((uint8_t)type << 4) | ((uint8_t)packageFlags & 0x0F);
        int fixedHeaderSize = EBMqtt::IntToVBI(size, fixedHeader + 1, 8) + 1;
        _tcpSocket.write((char*)fixedHeader, fixedHeaderSize);
        _tcpSocket.write((char*)buffer, size);
    }

    uint16_t getFreePacketIdentifier()
    {
        uint16_t id = 1;
        while( knownPacketIdentifier.contains(id) )
            id++;

        if( id != 0 )
        {
            knownPacketIdentifier.append(id);
        }

        return id;
    }

    void resetPacketIdentifier(uint16_t id)
    {
        knownPacketIdentifier.remove(id);
    }

    std::mutex mutex;

    EBList<uint16_t> knownPacketIdentifier;

    enum class STATE {
        PACKAGE_TYPE,
        LENGTH,
        DATA
    };

    struct RECEIVED_COMMAND{
        EBMqtt::PACKET_TYPE packageType;
        uint8_t packageTypeFlags;
        uint32_t length;
        uint32_t multiplier;

        STATE state;
        uint32_t dataPos;
        uint8_t data[1024*1024];
    };

    RECEIVED_COMMAND _receivedCommand;
};

}
