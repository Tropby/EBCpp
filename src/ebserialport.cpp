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
 *  Created on: Jul 9, 2020
 *      Author: Carsten (Tropby)
 */

#include "ebserialport.h"

#include <functional>

#ifdef __WIN32__
#include<windows.h>
#else
	#include <termios.h>
#endif

EBCpp::EBSerialPort::EBSerialPort() :
		EBFile(), pollThread(nullptr) {
}

EBCpp::EBSerialPort::~EBSerialPort() {
}

bool EBCpp::EBSerialPort::setInterfaceAttributes(int speed, int parity) {

#ifdef __WIN32__
	#warning "EBCpp::EBSerialPort::setInterfaceAttributes not working right now!"
	return false;
#else
	struct termios tty;
	if (tcgetattr(file, &tty) != 0) {
		return false;
	}

	cfsetospeed(&tty, speed);
	cfsetispeed(&tty, speed);

	tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;     // 8-bit chars
	// disable IGNBRK for mismatched speed tests; otherwise receive break
	// as \000 chars
	tty.c_iflag &= ~IGNBRK;         // disable break processing
	tty.c_lflag = 0;                // no signaling chars, no echo,
									// no canonical processing
	tty.c_oflag = 0;                // no remapping, no delays
	tty.c_cc[VMIN] = 0;            // read doesn't block
	tty.c_cc[VTIME] = 5;            // 0.5 seconds read timeout

	tty.c_iflag &= ~(IXON | IXOFF | IXANY); // shut off xon/xoff ctrl

	tty.c_cflag |= (CLOCAL | CREAD); // ignore modem controls,
									 // enable reading
	tty.c_cflag &= ~(PARENB | PARODD);      // shut off parity
	tty.c_cflag |= parity;
	tty.c_cflag &= ~CSTOPB;
	tty.c_cflag &= ~CRTSCTS;

	if (tcsetattr(file, TCSANOW, &tty) != 0) {
		return false;
	}
	return true;
#endif
}

void EBCpp::EBSerialPort::poll() {
	fd_set set;
	fd_set except;
	while (isOpen()) {
		int result = ::select(file, &set, nullptr, &except, nullptr);

		switch (result) {
		case 1:
			readReady.emit(this);
			break;
		default:
			break;
		}

	}
}

bool EBCpp::EBSerialPort::open(std::string filename) {
	if (!EBCpp::EBFile::open(filename))
		return false;

	pollThread = new std::thread( std::bind( &EBSerialPort::poll, this ) );

	return true;
}

bool EBCpp::EBSerialPort::close()
{
	if (!EBCpp::EBFile::close())
		return false;

	pollThread->join();
	delete pollThread;
	pollThread = nullptr;

	return true;
}
