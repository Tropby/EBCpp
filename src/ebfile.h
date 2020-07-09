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

#ifndef SRC_EBFILE_H_
#define SRC_EBFILE_H_

#ifdef __WIN32__
#include <direct.h>
#else
	#include <sys/stat.h>
#endif

#include <string>

namespace EBCpp {

class EBFile {
public:
	EBFile();
	virtual ~EBFile();

	/**
	 * Opens a file for I/O operations
	 */
	virtual bool open( std::string filename );

	/**
	 * Closes a file
	 */
	virtual bool close();

	/**
	 * Read raw data from the I/O device
	 */
	virtual unsigned int read( char * buffer, unsigned int size );

	/**
	 * Read a string from the I/O device
	 */
	virtual std::string readString();

	/**
	 * Write raw data to the I/O device
	 */
	virtual unsigned int write( const char * buffer, unsigned int size );

	/**
	 * Write a string to the I/O device
	 */
	virtual unsigned int write( std::string string );

	/**
	 * returns the I/O device state
	 */
	virtual bool isOpen();

protected:
	/**
	 * the file handle
	 */
	int file;
};

}

#endif /* SRC_EBFILE_H_ */
