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
 *  Created on: Jul 15, 2020
 *      Author: Carsten (Tropby)
 */

#include <sstream>
#include <iostream>
#include <algorithm>

#include "ebhttpheader.h"

using namespace EBCpp;

EBHTTPHeader::EBHTTPHeader() :
		protocol("HTTP/1.0"), statusCode(200), statusMessage(
				"OK"), method("GET"), path("/")
{
}

EBHTTPHeader::~EBHTTPHeader()
{
}

std::string EBCpp::EBHTTPHeader::getHeader()
{
	std::string returnValue = protocol + " " + std::to_string(statusCode) + " "
			+ statusMessage + "\r\n";

	for (auto it : data)
	{
		returnValue += it.first + ": " + it.second + "\r\n";
	}

	return returnValue;
}

std::string EBCpp::EBHTTPHeader::getValue(std::string key)
{
	return data[key];
}

std::string EBCpp::EBHTTPHeader::setValue(std::string key, std::string value)
{
	data[key] = value;
	return data[key];
}

void EBCpp::EBHTTPHeader::parse(std::string data)
{
	std::stringstream strstream(data);
	std::string line;

	bool first = true;

	while (getline(strstream, line))
	{

		if (line.back() == '\r')
			line.pop_back();

		if (first)
		{
			std::stringstream linesplit(line);
			int i = 0;
			std::string l;
			while (getline(linesplit, l, ' '))
			{
				switch (i++)
				{
				case 0:
					method = l;
					break;
				case 1:
					path = l;
					break;
				case 2:
					protocol = l;
					break;
				default:
					// ???
					break;
				}
			}

			first = false;
		}
		else
		{
			int delimiter = line.find(':');

			if (delimiter >= 0)
			{
				std::string key = line.substr(0, delimiter);
				std::string value = line.substr(delimiter + 2);

				std::transform(key.begin(), key.end(), key.begin(), ::tolower);

				std::pair<std::string, std::string> p(key, value);

				this->data.insert(p);
			}
		}
	}
}

const std::string& EBCpp::EBHTTPHeader::getMethod() const
{
	return method;
}

const std::string& EBCpp::EBHTTPHeader::getPath() const
{
	return path;
}

const std::string& EBCpp::EBHTTPHeader::getProtocol() const
{
	return protocol;
}
