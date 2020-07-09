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

#include "ebfile.h"

#include <io.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

EBCpp::EBFile::EBFile() : file(-1){
}

EBCpp::EBFile::~EBFile() {
}

bool EBCpp::EBFile::open(std::string filename)
{
	if( file == -1 )
		return false;

	int fd = ::open( filename.c_str(), _O_RDONLY );
	if( fd == -1 )
		return false;

	file = fd;
	return true;
}

bool EBCpp::EBFile::close() {
	if( file >= 0 )
	{
		::close(file);
		file = -1;
		return true;
	}
	return false;

}

unsigned int EBCpp::EBFile::read(char *buffer, unsigned int size)
{
	return ::read(file,  buffer,  size);
}

unsigned int EBCpp::EBFile::write(const char *buffer, unsigned int size)
{
	return ::write( file, buffer, size );
}

std::string EBCpp::EBFile::readString()
{
	char buffer[1024];
	int n = this->read( buffer, sizeof(buffer));
	return std::string( buffer, n );
}

unsigned int EBCpp::EBFile::write(std::string string) {
	return this->write( string.c_str(), string.length());
}

bool EBCpp::EBFile::isOpen()
{
	return (file != -1);
}
