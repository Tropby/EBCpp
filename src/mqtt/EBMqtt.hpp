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

#include <exception>

namespace EBCpp
{

class EBMqtt 
{
public: 
    enum class PACKET_TYPE  {
        CONNECT = 1,
        CONNACK = 2,
        PUBLISH = 3,
        PUBACK = 4,
        PUBREC = 5,
        PUBREL = 6,
        PUBCOMP = 7,
        SUBSCRIBE = 8,
        SUBACK = 9,
        UNSUBSCRIBE = 10,
        UNSUBACK = 11,
        PINGREQ = 12,
        PINGRESP = 13,
        DISCONNECT = 14,
        AUTH = 15
    };

    enum class CONNECT_FLAGS {
        USERNAME = (1<<7),
        PASSWORD = (1<<6),
        WILL_RETAIN = (1<<5),
        WILL_QOS10 = (1<<4),
        WILL_QOS01 = (1<<3),
        WILL_FLAG = (1<<2),
        CLEAN_START = (1<<1),
        RESERVED = (1<<0)
    };

    enum class RETAIN {
        YES = 1,
        NO = 0
    };

    enum class QOS {
        QOS0 = 0x00,
        QOS1 = 0x01,
        QOS2 = 0x02,
        QOS_RESERVERD = 0x03
    };

    enum class SUB_OPTION {
        QOS01 = (1<<0),
        QOS10 = (1<<1),
        NL = (1<<2),
        RAP = (1<<3),
        RH01 = (1<<4),
        RH10 = (1<<5)
    };

    enum class CONNACK_REASON_CODE {
        SUCCESS = 0x00,
        UNSPECIFIED_ERROR = 0x80,
        MALFORMED_PACKET = 0x81,
        PROTOCOL_ERROR = 0x82,
        IMPLEMENTATION_SPECIFIC_ERROR = 0x83,
        UNSUPPORTED_PROTOCOL_VERSION = 0x84,
        CLIENT_IDENTIFIER_NOT_VALID = 0x85,
        BAD_USER_NAME_OR_PASSWORD = 0x86,
        NOT_AUTHORIZED = 0x87,
        SERVER_UNAVAILABLE = 0x88,
        SERVER_BUSY = 0x89,
        BANNED = 0x8A,
        BAD_AUTHENTICATION_METHOD = 0x8C,
        TOPIC_NAME_INVALID = 0x90,
        PACKET_TOO_LARGE = 0x95,
        QUOTA_EXCEEDED = 0x97,
        PAYLOAD_FORMAT_INVALID = 0x99,
        RETAIN_NOT_SUPPORTED = 0x9A,
        QOS_NOT_SUPPORTED = 0x9B,
        USE_ANOTHER_SERVER = 0x9C,
        SERVER_MOVED = 0x9D,
        CONNECTION_RATE_EXCEEDED = 0x9F
    };

    enum class PUBACK_REASON_CODE {
        SUCCESS = 0x00,
        NO_MATCHING_SUBSCRIBERS = 0x10,
        UNSPECIFIED_ERRRO = 0x80,
        IMPLEMENTATION_SPECIFIC_ERROR = 0x83,
        NOT_AUTHORIZED = 0x87,
        TOPIC_NAME_INVALID = 0x90,
        PACKET_IDENTIFIER_IN_USE = 0x91,
        QUOTA_EXCEEDED = 0x97,
        PAYLOAD_FORMAT_INVALID = 0x99
    };

    enum class DISCONNECT_REASON_CODE {
        NORMAL_DISCONNECTION = 0x00,
        DISCONNECT_WITH_WILL_MESSAGE = 0x04,
        UNSPECIFIED_ERROR = 0x80,
        MALFORMED_PACKET = 0x81,
        PROTOCOL_ERROR = 0x82,
        IMPLEMENTATION_SPECIFIC_ERROR = 0x83,
        NOT_AUTHORIZED = 0x87,
        SERVER_BUSY = 0x89,
        SERVER_SHUTTING_DOWN = 0x8B,
        KEEP_ALIVE_TIMEOUT = 0x8D,
        SESSION_TAKEN_OVER = 0x8E,
        TOPIC_FILTER_INVALID = 0x8F,
        TOPIC_NAME_INVALID = 0x90,
        RECEIVE_MACIMUM_EXCEEDED = 0x93,
        TOPIC_ALIAS_INVALID = 0x94,
        PACKET_TOO_LARGE = 0x95,
        MESSAGE_RATE_TOO_HIGH = 0x96,
        QUOTA_EXCEEDE = 0x97,
        ADMINISTRATIVE_ACTION = 0x98,
        PAYLOAD_FORMAT_INVALID = 0x99,
        RETAIN_NOT_SUPPORTED = 0x9A,
        QOS_NOT_SUPPORTED = 0x9B,
        USE_ANOTHER_SERVER = 0x9C,
        SERVER_MOVED = 0x9D,
        CONNECTION_RATE_EXCEEDED = 0x9F,
        MAXIMUM_CONNECT_TIME = 0xA0,
        SUBSCRIBTION_IDENTIFIERS_NOT_SUPPORTED = 0xA1,
        WILDCARD_SUBSCRIPTIONS_NOT_SUPPORTED = 0xA2
    };

    static EBString getString(uint8_t * buffer, size_t size, uint32_t * pos )
    {
        EBString s;

        if( size < 2 )
        {
            throw new EBException("Size for string must be at least 2 byte");
        }
        *pos += 2;

        uint16_t si = buffer[0];
        si <<= 8;
        si += buffer[1];

        s = EBString((char*)buffer + 2, si);
        *pos += si;

        return s;
    }

    static int32_t VBIToInt(uint8_t * buffer, size_t size, uint32_t * value)
    {
        int multipier = 1;
        uint32_t pos = 0;
        uint8_t encodedByte;
        *value = 0;

        do 
        {
            pos++;
            encodedByte = *buffer;
            buffer++;

            *value += (encodedByte & 0x7F) * multipier;
            if( multipier > 0x80*0x80*0x80)
            {
                throw new std::exception();
            }

            multipier *= 0x80;
        } while ( (encodedByte & 0x80) != 0 );

        return pos;
    }

    static int IntToVBI(int32_t i, uint8_t * buffer, size_t size)
    {
        int count = 0;
        do{
            int encodedByte = i % 0x80;
            i = i / 0x80;

            if( i > 0 )
            {
                encodedByte |= 0x80;
            }
            *buffer = encodedByte;
            buffer++;
            size--;
            count++;
        } while (i > 0 && size > 0);

        return count;
    }

    static int string(const EBString& input, uint8_t * data, size_t size )
    {
        if( size < 2 )
        {
            throw new EBException("Size must be at least 2");
        }

        uint16_t len = input.length();
        data[0] = ( len >> 8 ) & 0xFF;
        data[1] = ( len >> 0 ) & 0xFF;

        size -= 2;

        if( size < len )
        {
            throw new EBException("String size to big!");
        }

        for (int i = 0; i < len; i++ )
        {
            data[i + 2] = input[i];
        }

        return len + 2;
    }
};

}