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

#include <iostream>
#include "../src/EBFile.hpp"

/**
 * @brief Main from the timer example
 *
 * @return int Exit code (0)
 */
int main()
{
    EBCpp::EBFile file;
    file.setFileName("../examples/testFile.txt");
    if (file.open(EBCpp::EBFile::READ_ONLY))
    {
        while (file.canReadLine())
        {
            std::cout << "Read one line: \"" << file.readLine() << "\" At End? " << file.atEnd() << " Can read Line? "
                      << file.canReadLine() << std::endl;
        }
    }
    else
    {
        std::cout << "Can not open the file: " << file.getFileName() << std::endl;
    }

    return 0;
}