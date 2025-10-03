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

#ifndef MODULE_ESP8266_H
#define MODULE_ESP8266_H

#include <ESP8266WiFi.h>

struct DataPacketOut
{
  IPAddress ipaddr;
  uint16_t co2_ppm;
  uint8_t rating;
};

struct Module_ESP8266
{
  Module_ESP8266(void);
  Module_ESP8266(ESP8266WiFiClass *WifiClass);

  int16_t TryConnectWPS(void);
  int16_t TryConnect(const char* SSID, const char* Password);
  int16_t TryServer(const char* ServerIP, uint16_t Port);
  int16_t TrySendDataPacketJSON(DataPacketOut packet);

  void printMAC(void);
  void printIP(void);

  IPAddress getIP(void);

  private:
      WiFiClient serverConnection;
      ESP8266WiFiClass *wifi_class;
      uint8_t wifi_mac[6];
};

#endif
