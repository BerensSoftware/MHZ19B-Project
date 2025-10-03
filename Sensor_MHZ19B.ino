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

#include "Sensor_MHZ19B.h"

static uint8_t MHZ19B_getCheckSum(uint8_t *bytes)
{
    uint8_t i, checksum = 0;
    for( i = 1; i < 8; i++)
        checksum += bytes[i];

    checksum = 0xff - checksum;
    checksum += 1;

    return checksum;
}

Sensor_MHZ19B::Sensor_MHZ19B(void) : serialPort(NULL) {}
Sensor_MHZ19B::Sensor_MHZ19B(uint8_t rx, uint8_t tx) 
{
    //evil
    this->serialPort = new SoftwareSerial(rx, tx);
    this->serialPort->begin(9600);
}


int16_t 
Sensor_MHZ19B::CleanSerial(void)
{
    if(this->serialPort == NULL) return MHZ19B_ERROR_SERIALNOTSET;

    while(this->serialPort->available() > 0)
        this->serialPort->read();

    return 0;
}


/*Sensor Stuff*/
int16_t 
Sensor_MHZ19B::ReadCO2(void)
{
    if(this->serialPort == NULL) return MHZ19B_ERROR_SERIALNOTSET;

    size_t writtenBytes, readBytes;
    uint8_t checkSum;
    uint8_t in_bytes[MHZ19B_CMD_LENGTH] = {};
    uint16_t out_co2;
    const uint8_t CMD_READ_CO2[MHZ19B_CMD_LENGTH] = {0xff, 0x01, 0x86, 0x00, 0x00, 0x00, 0x00, 0x00, 0x79};

    //Clean up previous messages
    this->CleanSerial();

    writtenBytes = this->serialPort->write(CMD_READ_CO2, sizeof(CMD_READ_CO2));

    if(writtenBytes != sizeof(CMD_READ_CO2)) return MHZ19B_ERROR_WRITE;

    //Delay a little bit
    this->serialPort->setTimeout(300);

    readBytes = this->serialPort->readBytes(in_bytes, sizeof(in_bytes));

    //if(readBytes != sizeof(in_bytes)) return MHZ19B_ERROR_READ;
    if(in_bytes[0] == 0xff && in_bytes[2] == 0x86) return MHZ19B_ERROR_CMDFAIL;

    //checksum
    checkSum = MHZ19B_getCheckSum(in_bytes);

    if(checkSum != in_bytes[8]) return MHZ19B_ERROR_CHECKSUM;

    //Gas concentration = high level *256+low level
    out_co2 = (uint16_t)in_bytes[2] << 8;
    out_co2 += in_bytes[3];

    return out_co2;
}
