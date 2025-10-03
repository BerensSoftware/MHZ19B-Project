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

#include "Perc_Buzzer.h"

#define PERC_BUZZER_NO_PIN 0xFFFF

Perc_Buzzer::Perc_Buzzer(void) : pin_buzzer(PERC_BUZZER_NO_PIN), shouldEmit(false) {}
Perc_Buzzer::Perc_Buzzer(uint8_t p) : pin_buzzer(p), shouldEmit(false) {
    pinMode(this->pin_buzzer, OUTPUT);
}


void
Perc_Buzzer::Emit(bool state)
{ 
    this->shouldEmit = state;
}

void
Perc_Buzzer::WaitForEmit(unsigned int freq, unsigned long duration)
{
    if(!this->shouldEmit) return;

    tone(this->pin_buzzer, freq);
    delay(duration);
    noTone(this->pin_buzzer);
    delay(duration);
}


void
Perc_Buzzer::LoudEmit(bool emit)
{
    digitalWrite(this->pin_buzzer, emit);
}
