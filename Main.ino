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

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
 
#include "Sensor_MHZ19B.h"
#include "Module_ESP8266.h"
#include "Perc_Buzzer.h"
#include "Perc_Led.h"

//Enable reading internal supply voltage from ADC 
ADC_MODE(ADC_VCC);

//Helper Defs:
#define DELAY(ms) delay(ms)
#define LOG(...) Serial.printf(__VA_ARGS__);
#define READVCC() ESP.getVcc()

//Debugging:
#define USE_DISPLAY
#define USE_NETWORK
#define USE_SENSOR
#define USE_LEDS
#define USE_BUZZER

//#define CLEAR_CONFIG
#define MAX_NETTRIES    3

//Connection Settings:
#define SERVER_HOST     "$API_URL$"   // (saved in environment variable)
#define SERVER_PORT     "$API_PORT$"  // (saved in environment variable)

//Pins:
#define PIN_BUZZER      16 // D0 
#define PIN_LED_GREEN   0  // D3      
#define PIN_LED_RED     2  // D4       
#define PIN_LED_BLUE    14 // D5    
#define PIN_RX          13 // D7
#define PIN_TX          15 // D8
#define PIN_NET         12 // D6


enum SensorRating
{
    SENSORRATING_FAIL = 0,
    SENSORRATING_PERFECT,
    SENSORRATING_OK,
    SENSORRATING_CRITICAL,
    SENSORRATING_WARNING,
    SENSORRATING_VERYCRITICAL,
    SENSORRATING_SIZE
};

#define ERRORSTATUS_NO_ERROR        0
#define ERRORSTATUS_NO_OLED         1
#define ERRORSTATUS_NO_SENSOR       2
#define ERRORSTATUS_NO_ACCESSPOINT  4
#define ERRORSTATUS_NO_SERVER       8

static uint16_t         g_co2_ppm;         // Global for current CO2 PPM value
static SensorRating     g_sensorrating;    // Global for CO2 PPM rating
static uint8_t          g_errorstatus;     // Global for current errors
static int              g_rssi;

static bool             net_mode;          // Netmode configurable through D6

static Module_ESP8266   wifiModule(&WiFi);
static Adafruit_SSD1306 display(128, 64, &Wire, -1);
static Perc_Buzzer      buzzer(PIN_BUZZER);
static Perc_Led         led(PIN_LED_GREEN, PIN_LED_RED, PIN_LED_BLUE);
static Sensor_MHZ19B    sensor_co2(PIN_RX, PIN_TX);

int16_t 
InitDisplay(void)
{
    if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) 
    {
        g_errorstatus |= ERRORSTATUS_NO_OLED;
        LOG("SSD1306 allocation failed\n");
        return -1;
    }

    g_errorstatus &= ~ERRORSTATUS_NO_OLED;

    return 0;
}

void 
NetworkTry(int i)
{
    display.clearDisplay();
    display.setCursor(0, 25); 
    display.printf("Connecting");
    display.setCursor(0, 45); 
    display.printf("Tries: %d", i);
    display.display();
}

void 
SensorTry(void)
{
    display.clearDisplay();
    display.setCursor(0, 32); 
    display.printf("Testing...");
    display.display();
}


void 
UpdateDisplay(void)
{
    display.clearDisplay();
    display.setTextColor(WHITE);
    display.setTextSize(1);
 
    display.setCursor(0, 0);
    display.printf("%s", SensorRatingToString());

    display.setCursor(96, 0);

    if(g_rssi == 0)
        display.printf(" ---");
    else if((g_rssi <= -86))
        display.printf(" /  ");
    else if(g_rssi <= -68 && (g_rssi >= -85))
        display.printf(" // ");
    else if(g_rssi >= -67)
        display.printf(" ///");

    LOG("%d", g_rssi);

    // Horizontal Line for top area
    display.drawFastHLine(0,10, 127, WHITE);

    if(g_errorstatus == ERRORSTATUS_NO_ERROR)
    {
        byte level = constrain(map(READVCC(), 1900, 3000, 0, 100),0,100);
        display.setCursor(0, 54);    
        display.printf("%d%c", level, '%');
        display.setCursor(0, 32);    
    }
    else
        display.setCursor(0, 13);    

    display.setTextSize(2);
    if(g_errorstatus & ERRORSTATUS_NO_SENSOR)
    {
        display.printf("CO2: FAIL");
    }
    else
    	display.printf("CO2: %d", g_co2_ppm);

    // Horizontal Line for bottom area
    if(g_errorstatus != ERRORSTATUS_NO_ERROR)
    {
        display.drawFastHLine(0,30, 127, WHITE);
        display.setCursor(0, 32);    
        display.setTextSize(1);
 
        if(g_errorstatus & ERRORSTATUS_NO_OLED)
            LOG("Error: NO OLED");
        if(g_errorstatus & ERRORSTATUS_NO_SENSOR)
            display.println("Error: SENSOR FAIL"); 
        if(g_errorstatus & ERRORSTATUS_NO_ACCESSPOINT)
            display.println("Error: NO ACCESSPOINT");   
        if(g_errorstatus & ERRORSTATUS_NO_SERVER)
            display.println("Error: NO SERVER");
    }

    display.display();
}

int16_t 
InitNetwork(void)
{
    int16_t Error = 0;

#ifdef CLEAR_CONFIG
    WiFi.disconnect();
    while(1) DELAY(500);
#endif

    // Try connect to Access Point
    if((Error = wifiModule.TryConnectWPS()) < 0)
    {
        g_errorstatus |= ERRORSTATUS_NO_ACCESSPOINT;
		LOG("ERROR CONNECTING TO ACCESS POINT: %d\n", Error);
        return Error;
    }
    
    // Try connect to Server
    if((Error = wifiModule.TryServer(SERVER_HOST, SERVER_PORT)) < 0)
    {
        g_errorstatus |= ERRORSTATUS_NO_SERVER;
		LOG("ERROR CONNECTING TO SERVER: %d\n", Error);
        return Error;
    }

    g_errorstatus &= ~ERRORSTATUS_NO_SERVER;
    g_errorstatus &= ~ERRORSTATUS_NO_ACCESSPOINT;

    return 0;
}

void 
UpdateNetwork(void)
{
    DataPacketOut packet = {};

    if (WiFi.status() != WL_CONNECTED)
    {    
        g_errorstatus |= ERRORSTATUS_NO_ACCESSPOINT;   
        return;
    }

    g_errorstatus &= ~ERRORSTATUS_NO_ACCESSPOINT;

    if(g_errorstatus & ERRORSTATUS_NO_SERVER)
    {      
        //Try connect to Server again
        if(wifiModule.TryServer(SERVER_HOST, SERVER_PORT) == 0)
            g_errorstatus &= ~ERRORSTATUS_NO_SERVER; //Clear error
    }

    packet.ipaddr = wifiModule.getIP();
    packet.co2_ppm = g_co2_ppm;
    packet.rating = g_sensorrating;

    if(wifiModule.TrySendDataPacketJSON(packet) < 0)
    {
        g_errorstatus |= ERRORSTATUS_NO_SERVER;
        return;
    }

    g_errorstatus &= ~ERRORSTATUS_NO_SERVER;

    g_rssi = WiFi.RSSI();
}

const char* 
SensorRatingToString(void)
{ 
    switch(g_sensorrating)
    {
        case SENSORRATING_FAIL:
            return "ERROR";
        case SENSORRATING_PERFECT:
            return "PERFECT";
        case SENSORRATING_OK:
            return "OKAY";
        case SENSORRATING_CRITICAL:
            return "CRITICAL";
        case SENSORRATING_WARNING:
            return "WARNING!";
        case SENSORRATING_VERYCRITICAL:
            return "VENTILATION!";
        default:
            return "???";
    }

    return "";
}

void 
UpdateRating(void)
{
    if( (g_co2_ppm >= 400) && (g_co2_ppm < 580))
        g_sensorrating = SENSORRATING_PERFECT;
    else if( (g_co2_ppm >= 580) && (g_co2_ppm < 650) )
        g_sensorrating = SENSORRATING_OK;
    else if( (g_co2_ppm >= 650) && (g_co2_ppm < 900) )
        g_sensorrating = SENSORRATING_CRITICAL;
    else if( (g_co2_ppm >= 900) && (g_co2_ppm < 1200) )
        g_sensorrating = SENSORRATING_WARNING;
    else if(g_co2_ppm >= 1200)
        g_sensorrating = SENSORRATING_VERYCRITICAL;

#ifdef USE_LEDS
    switch(g_sensorrating)
    {
        case SENSORRATING_PERFECT:
        case SENSORRATING_OK:
            led.SetOnlyGreen();
            break;
        case SENSORRATING_CRITICAL:
            led.SetOnlyBlue();
            break;
        case SENSORRATING_WARNING:
        case SENSORRATING_VERYCRITICAL:
            led.SetOnlyRed();
            for(int i = 0; i < 3; i++)
            {
                buzzer.LoudEmit(true);
                DELAY(300);
                buzzer.LoudEmit(false);
            }
            break;
        default:
            led.SetOnlyBlue();
    }
#endif
}

void
UpdateCO2(void)
{
    g_co2_ppm = sensor_co2.ReadCO2();
 
    //Sensor Read error
    if(g_co2_ppm <= 0)
    {
        g_errorstatus |= ERRORSTATUS_NO_SENSOR;
        return;
    }

    g_errorstatus &= ~ERRORSTATUS_NO_SENSOR;
}


/*-------------------------------
  SETUP - on initial start of µC
-------------------------------*/
void setup()
{
    //Debugging
    Serial.begin(115200);

    int netTries = 0;

    pinMode(PIN_NET, INPUT_PULLUP);
    net_mode = !digitalRead(PIN_NET);


#ifdef USE_DISPLAY
tryDisplay:
    LOG("Trying Display...\n");
    if(InitDisplay() < 0)
        goto tryDisplay;
    LOG("Display success!\n");
#endif

#ifdef USE_NETWORK
if(net_mode)
{
tryNetwork:
    NetworkTry(netTries);
    LOG("Trying Network...\n");
    if(InitNetwork() < 0)
    {
        if(netTries++ < MAX_NETTRIES)
            goto tryNetwork;
    }
    else 
        LOG("Network success!\n");
}
#endif

#ifdef USE_SENSOR
trySensor:
    SensorTry();
    UpdateCO2();
    if(!g_co2_ppm)
        goto trySensor;
#endif

}

/*-------------------------------
  LOOP - On every cpu cycle/tick
-------------------------------*/
void loop()
{

#ifdef USE_SENSOR
    UpdateCO2();
    UpdateRating();
#endif

#ifdef USE_DISPLAY
    UpdateDisplay();
#endif

#ifdef USE_NETWORK
if(net_mode)
    UpdateNetwork();
#endif

    delay(15000);
}
