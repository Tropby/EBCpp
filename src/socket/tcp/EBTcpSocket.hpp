/*
 * EBCpp
 *
 * Copyright (C) 2020 Carsten Grings
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 *  Created on: 2021-01-23
 *      Author: Carsten (Tropby)
 */

#pragma once

#include <atomic>

#include "../../EBEvent.hpp"
#include "../../EBException.hpp"
#include "../../EBIODevice.hpp"
#include "../../EBSemaphore.hpp"
#include "../../EBUtils.hpp"

#include "EBTcpHeader.hpp"

namespace EBCpp
{

/**
 * @brief Socket to handle a tcp connection
 *
 */
class EBTcpSocket : public EBIODevice
{
public:
    /**
     * @brief Construct a new EBTcpSocket object
     *
     * @param parent Parent of the EBTcpSocket instance
     */
    EBTcpSocket(EBObject* parent) : EBIODevice(parent), socketId(-1), thread(nullptr), connectionState(false)
    {
        static bool inited = false;
        if (!inited)
        {
#ifdef __WIN32__
            WORD versionWanted = MAKEWORD(1, 1);
            WSADATA wsaData;
            WSAStartup(versionWanted, &wsaData);
#endif
            inited = true;
        }
    }

    /**
     * @brief Construct a new EBTcpServerSocket object
     *
     * @param parent EBTcpServer that have created the object
     * @param socketId socket id of the tcp connection
     * @param client client informations
     */
    EBTcpSocket(EBObject* parent, SOCKET socketId, struct sockaddr_in client) :
        EBIODevice(parent), thread(nullptr), connectionState(true), socketId(socketId), address(client)
    {
#ifdef __WIN32__
        WORD versionWanted = MAKEWORD(1, 1);
        WSADATA wsaData;
        WSAStartup(versionWanted, &wsaData);
#endif
    }

    /**
     * @brief Destroy the EBTcpSocket object
     *
     */
    virtual ~EBTcpSocket()
    {
        close();
        joinThread();
    }

    /**
     * @brief Connect to a host set by "setFileName"
     *
     * @param direction Direction must be READ_WRITE for TCP connections
     * @return true if the socket could be created and an ip was received (connection will be handelt ba a thread)
     * @return false if the socket could not be created.
     * @throws EBException if something is wrong with the connection string (fileName)
     */
    virtual bool open(EBIODevice::DIRECTION direction)
    {

        if (direction != READ_WRITE)
            EB_EXCEPTION("Can not open a tcp socket read only or write only.");

        // Check if hostname can be used for this connection
        std::string host = getFileName();

        if (host.substr(0, 6).compare("tcp://"))
            EB_EXCEPTION("EBTcpSocket needs a tcp://{hostname}:{port} filename to connect to a host.");

        host = host.substr(6);

        if (host.find(':') == std::string::npos)
            EB_EXCEPTION("EBTcpSocket needs a tcp://{hostname}:{port} filename to connect to a host.");

        int32_t port = std::stoi(host.substr(host.find(':') + 1));
        host = host.substr(0, host.find(':'));

        if (port > 0xFFFF || port < 0)
            EB_EXCEPTION("EBTcpSocket port rage (0 - 65535).");

        // Get ip address from host name otherwise treat host as ip address
        std::string hostIp = EBUtils::hostnameToIp(host);
        if (!hostIp.empty())
            host = hostIp;

// convert ip address string to ip address
#ifdef __WIN32__
        memset(&address, 0, sizeof(SOCKADDR_IN));
        address.sin_family = AF_INET;
        address.sin_port = htons(port);
        address.sin_addr.s_addr = inet_addr(host.c_str());
#else
        inet_pton(AF_INET, host.c_str(), &address);
#endif

        startThread();

        return true;
    }

    /**
     * @brief Returns the current connection state
     *
     * @return true if the connection es established
     * @return false if the connection is closed
     */
    virtual bool isOpened()
    {
        return connectionState;
    }

    /**
     * @brief Closes the current connection
     *
     * @return true if the connection could be closed
     * @return false NEVER
     */
    virtual bool close()
    {
        if (isOpened())
        {
            connectionState = false;

#ifdef __WIN32__                    
                shutdown(socketId, SD_SEND);
#else                
                shutdown(socketId, SHUT_WR);
#endif                    

            return true;
        }
        return false;        
    }

    /**
     * @brief Send raw binary data
     *
     * @param data Pointer to the data
     * @param length Length of the data
     * @return int bytes written to the tcp socket
     */
    virtual int write(char* data, int length)
    {
        return send(socketId, data, length, 0);
    }

    /**
     * @brief Send string
     *
     * @param data string to send
     * @return int bytes written to the tcp socket
     */
    virtual int write(std::string data)
    {
        return send(socketId, data.c_str(), data.length(), 0);
    }

    /**
     * @brief Reads data from the tcp socket
     *
     * @param data Buffer for the received data
     * @param maxLength Buffer size
     * @return int bytes read from the tcp socket
     */
    virtual int read(char* data, int maxLength)
    {
        const std::lock_guard<std::mutex> lock(mutex);

        int size = this->data.size() < maxLength ? this->data.size() : maxLength;

        for (int i = 0; i < size; i++)
        {
            *data = this->data.front();
            data++;
            this->data.pop_front();
        }

        return size;
    }

    /**
     * @brief Checks if the data buffer contains "\\n"
     *
     * @return true if the data buffer contains "\\n"
     * @return false if the data buffer does not contain "\\n"
     */
    virtual bool canReadLine()
    {
        const std::lock_guard<std::mutex> lock(mutex);
        return (std::find(data.begin(), data.end(), '\n') != data.end());
    }

    /**
     * @brief Reads a line ending with "\\n" from the tcp socket
     *
     * @return std::string Line that was read from the tcp socket
     * @throws EBException if no line ending was found
     */
    virtual std::string readLine()
    {
        const std::lock_guard<std::mutex> lock(mutex);

        bool found = (std::find(data.begin(), data.end(), '\n') != data.end());
        if (found)
        {
            std::string result;
            char c = 0;
            while (c != '\n')
            {
                c = this->data.front();
                result += c;
                this->data.pop_front();
            }
            return result;
        }
        else
        {
            EB_EXCEPTION("Can not read line. No \\n found in received data!");
        }

        return std::string();
    }

    //! Starts the receiver thread
    void startThread()
    {
        thread = std::unique_ptr<std::thread>(new std::thread(std::bind(&EBTcpSocket::run, this)));
    }

    void joinThread()
    {
        if (thread)
        {
            if( thread->joinable() )
            {
                thread->join();
            }
            thread = nullptr;
        }
    }

    /**
     * @brief EB_SIGNAL error
     *
     * Emitted if an error occures on the TCP connection
     */
    EB_SIGNAL_WITH_ARGS(error, std::string);

    /**
     * @brief EB_SIGNAL connected
     *
     * Emitted if the connection was established
     */
    EB_SIGNAL(connected);

    /**
     * @brief EB_SIGNAL diconnected
     *
     * Emitted if the connection was terminated
     */
    EB_SIGNAL(disconnected);

    /**
     * @brief EB_SIGNAL readReady
     *
     * Emitted if new data is available
     */
    EB_SIGNAL(readReady);

protected:
    //! SocketId of the tcp connection
    SOCKET socketId;

    //! Client informations
    SOCKADDR_IN address;

    /**
     * @brief Creates a socket and connects to the host
     *
     * @return true if the socket is connected
     * @return false otherwise
     */
    virtual bool connect()
    {
        // Try to get a new socket
        socketId = ::socket(AF_INET, SOCK_STREAM, 0);
        if (socketId == -1)
        {
            EB_EMIT_WITH_ARGS(error, "Can not create socket!");
            return false;
        }

        // Try to connect to host
        int descriptor = ::connect(socketId, reinterpret_cast<sockaddr*>(&address), sizeof(address));
        if (descriptor == -1)
        {
            EB_EMIT_WITH_ARGS(error, "Can not connect to address!");
            return false;
        }
        connectionState = true;
        return true;
    }

    /**
     * @brief Receive data from the socket
     *
     * @param buffer buffer for the data
     * @param size size of the buffer
     * @return int bytes read from the socket
     */
    virtual int receiveData(char* buffer, int size)
    {
        // Read next block of data
        return ::recv(socketId, buffer, sizeof(buffer), 0);
    }

    /**
     * @brief Returns true if the end of stream is reached
     * 
     * @return true if at the end of the io stream
     * @return false otherwise
     */
    virtual bool atEnd()
    {
        return (data.size() == 0);
    }

private:
    std::unique_ptr<std::thread> thread;
    bool connectionState;
    std::list<char> data;
    std::mutex mutex;

    /**
     * This method is used for the thread.
     * Handle the connection, read events.
     **/
    void run()
    {
        EBUtils::setThreadName("TcpSocket #???");

        // Socket is allredy known and connected (eg. server sockets)
        if (socketId == -1)
        {
            if (!connect())
            {
                return;
            }
            connected.emit(this);

            // EB_EMIT(connected);
        }
        else
        {
            connectionState = true;
        }

        EBUtils::setThreadName(std::string("TcpSocket #") + std::to_string(socketId));

        // Read until the end of life
        char buffer[1024];
        int nbytes;
        while (true)
        {
            nbytes = receiveData(buffer, sizeof(buffer));

            switch (nbytes)
            {
            // Discriptor disconnected
            case 0:
#ifdef __WIN32__                    
                    ::closesocket(socketId);
#else                
                    ::close(socketId);
#endif                    

                EB_EMIT(disconnected);
                return;

            case -1:
#ifdef __WIN32__                    
                ::closesocket(socketId);
#else                
                ::close(socketId);
#endif                    

                // if not closed send error message otherwise send disconnected
                if (connectionState)
                {
                    EB_EMIT_WITH_ARGS(error, "Error while reading from socket!");
                }
                else
                {
                    EB_EMIT(disconnected);
                }
                return;

            default:
                std::vector<uint8_t> b(buffer, buffer + nbytes);
                mutex.lock();
                data.insert(data.end(), std::begin(b), std::end(b));
                mutex.unlock();
                EB_EMIT(readReady);
            }
        }

        return;
    }
};

} // namespace EBCpp