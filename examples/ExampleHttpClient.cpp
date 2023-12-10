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
        client.error.connect(this, &ExampleHttpClient::httpRequestError);
        get();
    }

    void get()
    {
        if( !client.get("http://192.168.0.204/cm?cmnd=power0") )
        {
            EB_LOG_DEBUG("Can not start GET request!");
        }
    }

private:
    EBCpp::EBHttpClient<> client;

    EB_SLOT(httpRequestError)
    {
        EB_LOG_DEBUG("Error! " << client.getErrorMessage());
        get();
    }

    EB_SLOT(httpRequestFinished)
    {
        EBCpp::EBString strData = client.getResult();
        EB_LOG_DEBUG("Result: " << strData);
        
        //client.finished.disconnect(this, &ExampleHttpClient::httpRequestFinished);
        //EBCpp::EBEventLoop::getInstance()->exit();

        get();
    }
};

int main()
{
    ExampleHttpClient exampleHttpClient;
    EBCpp::EBEventLoop::getInstance()->exec();
}
