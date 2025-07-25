/*************************************************************
  Download latest Blynk library here:
    https://github.com/blynkkk/blynk-library/releases/latest

  Blynk is a platform with iOS and Android apps to control
  Arduino, Raspberry Pi and the likes over the Internet.
  You can easily build graphic interfaces for all your
  projects by simply dragging and dropping widgets.

    Downloads, docs, tutorials: http://www.blynk.cc
    Sketch generator:           http://examples.blynk.cc
    Blynk community:            http://community.blynk.cc
    Follow us:                  http://www.fb.com/blynkapp
                                http://twitter.com/blynk_app

  Blynk library is licensed under MIT license
  This example code is in public domain.

 *************************************************************
  This example runs directly on ESP8266 chip.

  Note: This requires ESP8266 support package:
    https://github.com/esp8266/Arduino

  Please be sure to select the right ESP8266 module
  in the Tools -> Board menu!

  Change WiFi ssid, pass, and Blynk auth token to run :)
  Feel free to apply it to any other example. It's simple!
 *************************************************************/

/* Comment this out to disable prints and save space */

#define BLYNK_TEMPLATE_ID           "TMPLcmNcMa75"
#define BLYNK_TEMPLATE_NAME      "IOT HOME AUTOMATION"
#define BLYNK_FIRMWARE_VERSION          "0.1.0"

// #define BLYNK_PRINT Serial

#include <TimeLib.h>
#include "hw_timer.h" 
#include "BlynkEdgent.h"
#include <Simpletimer.h>
#include <WidgetRTC.h>
#include <TimeLib.h>

// #include <ESP8266WiFi.h>
// #include <BlynkSimpleEsp8266.h>
        
const byte zcPin = 1;
const byte pwmPin = 3;  

byte fade = 1;
byte state = 1;
byte tarBrightness = 255;
byte curBrightness = 0;
byte zcState = 0;                                    // 0 = ready; 1 = processing;

int Slider_Value = 0;

const int LightPin = 0;
const int BulbPin = 2;

int LightState = LOW;
int BulbState = LOW;

void zcDetectorISR();
void dimTimerISR();
 
BLYNK_WRITE(V0)                                      // function to assign value to variable Slider_Value whenever slider changes position
   {Slider_Value = param.asInt();                    // assigning incoming value from pin V1 to a variable 
      if (Slider_Value > 0)
         {tarBrightness = Slider_Value;}} 

BLYNK_CONNECTED()                                    
  {Blynk.syncVirtual(V1);
   Blynk.syncVirtual(V2);}
  // Request the latest state from the server
  // Alternatively, you could override server state using:
  // Blynk.virtualWrite(V2, ledState);

BLYNK_WRITE(V1) 
  {LightState = param.asInt();
   digitalWrite(LightPin, LightState);}
  
BLYNK_WRITE(V2) 
  {BulbState = param.asInt();
   digitalWrite(BulbPin, BulbState);}

void zcDetectISR() 

 {if (zcState == 0) 
   
     {zcState = 1;
  
      if (curBrightness < 255 && curBrightness > 1) 
    
         {digitalWrite(pwmPin, 0);              
          int dimDelay = 30 * (255 - curBrightness) + 400;
          hw_timer_arm(dimDelay);}}}

void setup() 

 {Serial.begin(9600);  
  BlynkEdgent.begin();
  
  pinMode(BulbPin, OUTPUT);
  pinMode(LightPin, OUTPUT);

  digitalWrite(BulbPin, BulbState); 
  digitalWrite(LightPin, LightState);
    
  pinMode(zcPin, LOW);
  pinMode(pwmPin, OUTPUT);
  
  attachInterrupt(zcPin, zcDetectISR, RISING);     // Attach an Interupt to Pin 2 (interupt 0) for Zero Cross Detection
  hw_timer_init(NMI_SOURCE, 0);
  hw_timer_set_func(dimTimerISR);}

void loop() 
 
 {// timer1.run();
  BlynkEdgent.run();}
  
void dimTimerISR()
 
   {if (fade == 1) 
    
      {if (curBrightness > tarBrightness || (state == 0 && curBrightness > 0)) 
          {--curBrightness;}
       else if (curBrightness < tarBrightness && state == 1 && curBrightness < 255 && Slider_Value > 1) 
               {++curBrightness;}}

    else 
    
      {if (state == 1) 
          {curBrightness = tarBrightness;}     
       else 
          {curBrightness = 0;}}
    
    if (curBrightness <= 1) 
       {digitalWrite(pwmPin, 0);}
    
    else if (curBrightness == 255) 
            {state = 1;
             digitalWrite(pwmPin, 1);}

    else 
        {digitalWrite(pwmPin, 1);}
    
    zcState = 0;} 
