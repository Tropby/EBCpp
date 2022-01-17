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

#include <functional>
#include <iostream>
#include <string>

#include "../src/EBEvent.hpp"
#include "../src/EBEventLoop.hpp"
#include "../src/EBTimer.hpp"
#include "../src/http/EBHttpClient.hpp"

class ExampleHttpClient : public EBCpp::EBObject<ExampleHttpClient>
{
public:
    ExampleHttpClient()
    {
        client.finished.connect(this, &ExampleHttpClient::httpRequestFinished);
        client.get("http://192.168.222.44/cm?cmnd=status%200");
    }

private:
    EBCpp::EBHttpClient client;

    EB_SLOT(httpRequestFinished)
    {
        std::vector<char> data = client.getResult();
        std::string strData(data.begin(), data.end());
        EB_LOG_DEBUG("Result: " << strData);
        
        client.finished.disconnect(this, &ExampleHttpClient::httpRequestFinished);
        EBCpp::EBEventLoop::getInstance()->exit();
    }
};

int main()
{
    ExampleHttpClient exampleHttpClient;
    EBCpp::EBEventLoop::getInstance()->exec();
}
