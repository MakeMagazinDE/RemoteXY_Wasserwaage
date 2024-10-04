/*
   -- WasserWaage_Cloud_Make --
   
   This source code of graphical user interface 
   has been generated automatically by RemoteXY editor.
   To compile this code using RemoteXY library 3.1.13 or later version 
   download by link http://remotexy.com/en/library/
   To connect using RemoteXY mobile app by link http://remotexy.com/en/download/                   
     - for ANDROID 4.13.13 or later version;
     - for iOS 1.10.3 or later version;
    
   This source code is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.    
*/

//////////////////////////////////////////////
//        RemoteXY include library          //
//////////////////////////////////////////////

// you can enable debug logging to Serial at 115200
//#define REMOTEXY__DEBUGLOG    

// RemoteXY select connection mode and include library 
#include <Adafruit_NeoPixel.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <uri/UriBraces.h>

#define REMOTEXY_MODE__WIFI_CLOUD
#define WIFI_CHANNEL 6
// RemoteXY connection settings 
#define REMOTEXY_WIFI_SSID "Wokwi-GUEST"
#define REMOTEXY_WIFI_PASSWORD ""
#define REMOTEXY_CLOUD_SERVER "cloud.remotexy.com"
#define REMOTEXY_CLOUD_PORT 6376
#define REMOTEXY_CLOUD_TOKEN "eigenerToken"

#include <RemoteXY.h>

// RemoteXY GUI configuration  
#pragma pack(push, 1)  
uint8_t RemoteXY_CONF[] =   // 111 bytes
  { 255,13,0,26,0,104,0,17,0,0,0,29,1,106,200,1,1,7,0,11,
  6,7,20,20,7,71,14,99,81,81,59,0,2,24,255,0,0,0,0,0,
  0,180,67,0,0,52,66,0,0,112,65,0,0,160,64,24,0,67,49,27,
  40,10,4,25,30,11,67,49,41,40,10,4,25,30,11,129,18,27,20,9,
  25,112,105,116,99,104,0,129,18,41,13,9,25,114,111,108,108,0,1,20,
  66,69,14,1,2,31,122,101,114,111,0 };
  
// this structure defines all the variables and events of your control interface 
struct {

    // input variables
  float orientation_pitch; // from -PI to PI
  float orientation_roll; // from -PI to PI
  float orientation_yaw; // from -PI to PI
  uint8_t button_zero; // =1 if button pressed, else =0

    // output variables
  float instrument_winkel; // from 0 to 360
  char text_pitch[11]; // string UTF8 end zero
  char text_roll[11]; // string UTF8 end zero

    // other variable
  uint8_t connect_flag;  // =1 if wire connected, else =0

} RemoteXY;   

#pragma pack(pop)
 
/////////////////////////////////////////////
//           END RemoteXY include          //
/////////////////////////////////////////////

#include <Adafruit_NeoPixel.h>

#define PIN 25
#define NUM_LEDS 16

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, PIN, NEO_GRB + NEO_KHZ800);

float offset_roll = 0;
float offset_pitch = 0;
float flt_roll = 0;
float flt_pitch = 0;

void setup() 
{
  RemoteXY_Init(); 
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  
  // TODO your setup code
}

void loop() 
{ 
  // Datenaustausch mit RemoteXY App
  RemoteXY_Handler();
  
  // Speicherung des aktuellen Offsets wenn Zero Button gedrückt wird
  if (RemoteXY.button_zero) {
    offset_roll = RemoteXY.orientation_roll;
    offset_pitch = RemoteXY.orientation_pitch;
  }
  
  // Berechnung der Neigung und des Sturzes in RAD abzüglich der Nullpunktmessung
  flt_pitch = RemoteXY.orientation_pitch - offset_pitch;
  flt_roll = RemoteXY.orientation_roll - offset_roll;
  
  // Richtung der Neigung über ArcTan berechnen 
  RemoteXY.instrument_winkel = atan2(flt_roll,-flt_pitch) * 180.0 / PI +180;

  // Werte in Textfelder der App schreiben
  dtostrf(flt_pitch * 180.0 / PI, 10, 4, RemoteXY.text_pitch);
  dtostrf(flt_roll * 180.0 / PI, 10, 4, RemoteXY.text_roll);

  // Update NeoPixel ring
  int ledIndex = map(RemoteXY.instrument_winkel, 0, 360, 0, NUM_LEDS - 1);
  for (int i = 0; i < NUM_LEDS; i++) {
    if (i == ledIndex) {
      strip.setPixelColor(i, strip.Color(255, 0, 0)); // Bright red
    } else {
      strip.setPixelColor(i, strip.Color(0, 0, 0)); // Off
    }
  }
  strip.show();

  // Zur Verzögerung nicht delay() sonder RemoteXY_delay() nutzen
  RemoteXY_delay(100);
}
