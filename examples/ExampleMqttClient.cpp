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
 *  Created on: 2024-10-12
 *      Author: Carsten (Tropby)
 */

#include "../src/mqtt/EBMqttClient.hpp"
#include "../src/mqtt/EBMqtt.hpp"

class ExampleMqttClient : public EBCpp::EBObject<ExampleMqttClient>
{
public:
    ExampleMqttClient()
    {
        mqttClient.connack.connect(this, &ExampleMqttClient::connack);
        mqttClient.disconnect.connect(this, &ExampleMqttClient::disconnect);
        mqttClient.suback.connect(this, &ExampleMqttClient::suback);
        mqttClient.puback.connect(this, &ExampleMqttClient::puback);

        mqttClient.setUrl("tcp://192.168.222.59:1883/");
        mqttClient.setUsername("USERNAME");
        mqttClient.setPassword("PASSWORD");
        mqttClient.setClientId("ExampleMqttClient");
        mqttClient.connectToHost();
    }

    EB_SLOT_WITH_ARGS(puback, uint16_t packetId)
    {
        EB_LOG_INFO("PubAck received: " << packetId);
    }

    EB_SLOT_WITH_ARGS(suback, uint16_t packetId)
    {
        EB_LOG_INFO("Subscribe OKAY: " << packetId);
    }

    EB_SLOT_WITH_ARGS(connack, EBCpp::EBMqtt::CONNACK_REASON_CODE reasonCode)
    {
        EB_LOG_INFO("CONNACK [Reason Code: " << (int)reasonCode << "]");
        EB_LOG_INFO("Subscribe: " << mqttClient.subscribe("/#") );

        uint16_t id = mqttClient.publish("/test/Test01", "HALLO!", EBCpp::EBMqtt::QOS::QOS1);
        EB_LOG_INFO("Send Publish width CODE: " << id);
    }

    EB_SLOT_WITH_ARGS(disconnect, EBCpp::EBMqtt::DISCONNECT_REASON_CODE reasonCode)
    {
        EB_LOG_INFO("DISCONNECT [Reason Code: " << (int)reasonCode << "]");
    }

private:
    EBCpp::EBMqttClient<> mqttClient;
};

int main()
{
    ExampleMqttClient exampleMqttClient;
    EBCpp::EBEventLoop::getInstance()->exec();
}
