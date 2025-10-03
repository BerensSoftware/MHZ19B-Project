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

#ifndef SENSOR_MHZ19B_H
#define SENSOR_MHZ19B_H

#include <SoftwareSerial.h>

#define MHZ19B_CMD_LENGTH    9

#define MHZ19B_ERROR_SERIALNOTSET      -1
#define MHZ19B_ERROR_WRITE             -2
#define MHZ19B_ERROR_READ              -3
#define MHZ19B_ERROR_CMDFAIL           -4
#define MHZ19B_ERROR_CHECKSUM          -5

#define MHZ19B_CHECK(func) (func < 0)

struct Sensor_MHZ19B
{
    Sensor_MHZ19B(void);
    Sensor_MHZ19B(uint8_t rx, uint8_t tx);

    int16_t CleanSerial(void);

    /*Sensor Stuff*/
    int16_t ReadCO2(void);

private:
    SoftwareSerial *serialPort;
};

#endif
