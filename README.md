# MHZ19B-Project
This is the source code of my battery powered CO2 Sensor Project from 2020.

## Features
- Displays the concentration of CO₂ in a room in ppm on an OLED screen.
- Sends the sensor values to a backend API.
- Indicates the air quality with an RGB LED
- Uses WPS to establish a connection

## Components
* ESP8266 (Microcontroller)
* SSD1306 (OLED Display)
* MHZ19B (CO2 Sensor)
* Piezo 
* RGB Led

## Pin layout

| PIN  | Description |
| ------------- | ------------- |
| D0(16)  | Piezo + |
| D1(5)  | SCL (SSD1306) |
| D2(4)  |SDA (SSD1306) |
| D3(0)  | LED green |
| D4(2)  | LED red |
| D5(14)  | LED blue |
| D7(13)  | TX (MHZ19B) |
| D8(15)  | RX (MHZ19B) |

## Dependencies
* esp8266 Community
* Adafruit SSD1306

## License

Copyright (c) Daniel B. All rights reserved.

Licensed under the [MIT](LICENSE) license.
