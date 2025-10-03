/*
  MIT License

  Copyright (c) 2020 Daniel Berens

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
*/

#include "Module_ESP8266.h"

#define ESP8266_ERROR_NOCLASS_SET   -1
#define ESP8266_ERROR_TIMEOUT       -2
#define ESP8266_ERROR_SERVER        -3

#define WAIT_DELAY 500

Module_ESP8266::Module_ESP8266(void)
{
    this->wifi_class = nullptr;
    memset(this->wifi_mac, 0, sizeof(this->wifi_mac));
}

Module_ESP8266::Module_ESP8266(ESP8266WiFiClass *wclass) : wifi_class(wclass)
{
    memset(this->wifi_mac, 0, sizeof(this->wifi_mac));
}

int16_t 
Module_ESP8266::TryConnect(const char* ssid, const char* pass)
{
    if(this->wifi_class == nullptr) return ESP8266_ERROR_NOCLASS_SET;

    // We start by connecting to a WiFi network
    Serial.print("Connecting to: ");
    Serial.println(ssid);

    //Manually configurate wifi modules ip, gateway and subnet
    //this->wifi_class->config(ip, gateway, subnet); 

    //Try to connect to Wifi Access Point
    this->wifi_class->begin(ssid, pass);

    uint32_t start = millis();
    uint32_t end = start + 1000*10; //10 Seconds
    
    while (this->wifi_class->status() != WL_CONNECTED) 
    {
        delay(WAIT_DELAY);
        Serial.print(".");

        if(millis() >= end)
            return ESP8266_ERROR_TIMEOUT;
    }

    this->wifi_class->macAddress(this->wifi_mac);

    this->printMAC();
    this->printIP();

    return 0;
}

int16_t 
Module_ESP8266::TryConnectWPS(void)
{
    if(this->wifi_class == nullptr) return ESP8266_ERROR_NOCLASS_SET;

    //Station mode for WPS
    this->wifi_class->mode(WIFI_STA);
    delay(1000);
    
    this->wifi_class->begin("","");
   
    delay(4000);

    //Check if WiFi is already connected
    if (this->wifi_class->status() != WL_CONNECTED) 
    {
        Serial.println("\nAttempting connection ...");
        this->wifi_class->beginWPSConfig();

        uint32_t start = millis();
        uint32_t end = start + 1000*10; //10 Seconds

        while (this->wifi_class->status() != WL_CONNECTED) 
        {
            delay(WAIT_DELAY);
            Serial.print(".");

            if(millis() >= end)
                return ESP8266_ERROR_TIMEOUT;
        }

        if (this->wifi_class->status() == WL_CONNECTED) 
        {
            Serial.println("Connected!");
            Serial.println(this->wifi_class->localIP());
            Serial.println(this->wifi_class->SSID());
            Serial.println(this->wifi_class->macAddress());
        }
        else 
        {
            Serial.println("Connection failed!");
            return ESP8266_ERROR_TIMEOUT;
        }
    }
    else 
    {
        Serial.println("\nConnection already established.");
    }

    this->wifi_class->macAddress(this->wifi_mac);

    this->printMAC();
    this->printIP();

    return 0;
}

int16_t
Module_ESP8266::TryServer(const char* ServerIP, uint16_t Port)
{
    if(!serverConnection.connect(ServerIP, Port)) 
        return ESP8266_ERROR_SERVER;

    return 0;
}

int16_t 
Module_ESP8266::TrySendDataPacketJSON(DataPacketOut packet)
{
    if(!serverConnection.connected()) return ESP8266_ERROR_SERVER;

    //Send packet to backend API
    serverConnection.printf("{ \"from_ip\": \"%d.%d.%d.%d\", \"co2_ppm\": %d, \"status\": %d }", packet.ipaddr[0], packet.ipaddr[1], packet.ipaddr[2], packet.ipaddr[3], packet.co2_ppm, packet.rating);
    
    delay(500);

    String ans; 
    while (serverConnection.available())
        ans = serverConnection.readStringUntil('\r');

    if(ans.equals("SERVEROK"))
        Serial.printf("ANS: %s\n", ans.c_str());
    else
        Serial.printf("ERROR!!!!\n", ans.c_str());

    return 0;
}

void 
Module_ESP8266::printMAC(void)
{
    Serial.print("MAC: ");
    Serial.print(this->wifi_mac[5],HEX);
    Serial.print(":");
    Serial.print(this->wifi_mac[4],HEX);
    Serial.print(":");
    Serial.print(this->wifi_mac[3],HEX);
    Serial.print(":");
    Serial.print(this->wifi_mac[2],HEX);
    Serial.print(":");
    Serial.print(this->wifi_mac[1],HEX);
    Serial.print(":");
    Serial.println(this->wifi_mac[0],HEX);
    Serial.println("");
}


void 
Module_ESP8266::printIP(void)
{
    Serial.print("Assigned IP: ");
    Serial.print(this->getIP());
    Serial.println("");
}

IPAddress 
Module_ESP8266::getIP(void)
{
    if(this->wifi_class == nullptr)
        return ESP8266_ERROR_NOCLASS_SET;

    return this->wifi_class->localIP();
}
