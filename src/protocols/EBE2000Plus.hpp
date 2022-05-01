/*
 * EBCpp
 *
 * Copyright (C) 2022 Carsten (Tropby)
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
 *  Created on: 2022-04-28
 *      Author: Carsten (Tropby)
 */
#pragma once

#include "../EBEvent.hpp"
#include "../EBObject.hpp"
#include "../serialport/EBSerialPort.hpp"

namespace EBCpp
{

#define E2000_PLUS_START_BYTE 0x02

#define E2000_PLUS_MAX_TELEGRAM_LENGTH 64
typedef struct
{
    uint8_t address;
    uint8_t command;
    uint8_t length;
    uint8_t data[E2000_PLUS_MAX_TELEGRAM_LENGTH];
} E2000_PLUS_TELEGRAM;

typedef enum
{
    E2000_PLUS_RECEIVE_START_BYTE,
    E2000_PLUS_RECEIVE_ADDRESS_BYTE,
    E2000_PLUS_RECEIVE_COMMAND_BYTE,
    E2000_PLUS_RECEIVE_LENGTH_BYTE,
    E2000_PLUS_RECEIVE_DATA,
    E2000_PLUS_RECEIVE_CRC
} E2000_PLUS_RECEIVE_STATE;

class EBE2000Plus : public EBObject<EBE2000Plus>
{
public:
    EBE2000Plus()
    {
        serialPort.readReady.connect(this, &EBE2000Plus::dataReceived);
    }

    void setSerialPort(uint8_t serialPort)
    {
        this->serialPort.setPort(serialPort);
    }

    bool open()
    {
        return serialPort.open(EBSerialPort::READ_WRITE);
    }

    bool close()
    {
        return serialPort.close();
    }

    EB_SIGNAL_WITH_ARGS(telgramReceived, E2000_PLUS_TELEGRAM);
    EB_SIGNAL_WITH_ARGS(error, EBString);

private:
    EBSerialPort serialPort;

    E2000_PLUS_RECEIVE_STATE receiveState = E2000_PLUS_RECEIVE_START_BYTE;
    uint8_t receiveDataPosition;
    E2000_PLUS_TELEGRAM receiveTelegram;

    EB_SLOT(dataReceived)
    {
        uint8_t d;
        while (serialPort.read(reinterpret_cast<char*>(&d), 1))
        {

            switch (receiveState)
            {
            case E2000_PLUS_RECEIVE_START_BYTE:
                if (d == E2000_PLUS_START_BYTE)
                {
                    receiveState = E2000_PLUS_RECEIVE_ADDRESS_BYTE;
                }
                break;

            case E2000_PLUS_RECEIVE_ADDRESS_BYTE:
                receiveTelegram.address = d;
                receiveState = E2000_PLUS_RECEIVE_COMMAND_BYTE;
                break;

            case E2000_PLUS_RECEIVE_COMMAND_BYTE:
                receiveTelegram.command = d;
                receiveState = E2000_PLUS_RECEIVE_LENGTH_BYTE;
                break;

            case E2000_PLUS_RECEIVE_LENGTH_BYTE:
                receiveTelegram.length = d;

                if (receiveTelegram.length > E2000_PLUS_MAX_TELEGRAM_LENGTH)
                {
                    EB_EMIT_WITH_ARGS(error, EBString("Received telegram was to long"));
                }
                else
                {
                    receiveDataPosition = 0;
                    if (receiveTelegram.length > 0)
                        receiveState = E2000_PLUS_RECEIVE_DATA;
                    else
                        receiveState = E2000_PLUS_RECEIVE_CRC;
                }
                break;

            case E2000_PLUS_RECEIVE_DATA:
                receiveTelegram.data[receiveDataPosition] = d;
                receiveDataPosition++;
                if (receiveDataPosition == receiveTelegram.length)
                {
                    receiveState = E2000_PLUS_RECEIVE_CRC;
                }
                break;

            case E2000_PLUS_RECEIVE_CRC: {
                uint8_t crc = calCRC8((uint8_t*)&receiveTelegram, receiveTelegram.length + 3);
                if (crc == d)
                {
                    EB_EMIT_WITH_ARGS(telgramReceived, receiveTelegram);
                }
                else
                {
                    EB_EMIT_WITH_ARGS(error, EBString("Received telegram with wrong CRC"));
                }
                receiveState = E2000_PLUS_RECEIVE_START_BYTE;
            }
            break;
            }
        }
    }

    unsigned char calCRC8(unsigned char* data, unsigned char length)
    {
        unsigned char bit_counter;
        unsigned char feedback_bit;
        unsigned char i;
        unsigned char crc = 0xFF;
        unsigned char inByte;

        for (i = 0; i < length; i++)
        {
            bit_counter = 8;
            inByte = *data;
            data++;
            do
            {
                feedback_bit = (crc ^ inByte) & 0x01;
                if (feedback_bit)
                    crc = crc ^ 0x18;
                crc = (crc >> 1) & 0x7F;
                if (feedback_bit)
                    crc = crc | 0x80;
                inByte >>= 1;
                bit_counter--;
            } while (bit_counter > 0);
        }
        return crc;
    }
};

} // namespace EBCpp
