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

#include "Perc_Led.h"

#define PERC_LED_NO_PIN 0xFFFF

Perc_Led::Perc_Led(void) : pin_red(PERC_LED_NO_PIN), pin_green(PERC_LED_NO_PIN), pin_blue(PERC_LED_NO_PIN) {}
Perc_Led::Perc_Led(uint8_t g, uint8_t r, uint8_t b) : pin_red(r), pin_green(g), pin_blue(b) {
    pinMode(this->pin_green, OUTPUT);
    pinMode(this->pin_red, OUTPUT);
    pinMode(this->pin_blue, OUTPUT);
}

void 
Perc_Led::SetOnlyRed(void)
{
    digitalWrite(this->pin_red, HIGH);
    digitalWrite(this->pin_green, LOW);
    digitalWrite(this->pin_blue, LOW);
}

void 
Perc_Led::SetOnlyGreen(void)
{
    digitalWrite(this->pin_green, HIGH);
    digitalWrite(this->pin_blue, LOW);
    digitalWrite(this->pin_red, LOW);
}

void 
Perc_Led::SetOnlyBlue(void)
{
    digitalWrite(this->pin_green, LOW);
    digitalWrite(this->pin_blue, HIGH);
    digitalWrite(this->pin_red, LOW);
}

void 
Perc_Led::Clear(void)
{
    if((this->pin_red == PERC_LED_NO_PIN) || (this->pin_blue == PERC_LED_NO_PIN) || (this->pin_green == PERC_LED_NO_PIN)) return;

    digitalWrite(this->pin_red, LOW);
    digitalWrite(this->pin_green, LOW);
    digitalWrite(this->pin_blue, LOW);
}
