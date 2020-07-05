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
 *  Created on: Jul 3, 2020
 *      Author: Carsten (Tropby)
 */


/**
 * Information:
 * 		For Linux
 *	 		add "-I/{OPENSSL_PATH}/include" to Compiler
 * 			add "-lssl -lcrypto" to Linker
 * 			add "-L/{OPENSSL_PATH}/lib" to Linker
 *
 * 		For Windows
 * 			add "-I/{OPENSSL_PATH}/include" to Compiler
 * 			add "-lssl -lcrypto" to linker
 * 			add "-L/{OPENSSL_PATH}/lib" to Linker
 * 			copy "{OPENSSL_PATH}/bin/libssl-1_1-*.dll" to output directory (exe path)
 * 			copy "{OPENSSL_PATH}/bin/libcrypto-1_1-*.dll" to output directory (exe path)
 *
 */
// #define EB_OPENSSL


//////////////////////////////////////////////////////////////////////////

#include <iostream>
