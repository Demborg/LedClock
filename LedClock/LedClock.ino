// Library for neopixel led strand
#include <Adafruit_NeoPixel.h>

// Date and time functions using a DS3231 RTC connected via I2C and Wire lib
#include <Wire.h>
#include "RTClib.h"

// Hard coded parameters like pins etc.
#define STRIP 6

// Initialize the strip with 144 leds
Adafruit_NeoPixel strip = Adafruit_NeoPixel(144, STRIP, NEO_GRB + NEO_KHZ800);

// DS3231 RTC unit
RTC_DS3231 rtc;

void setup() {
  Serial.begin(9600);

  delay(3000); // wait for console opening

  // Connect to rtc on i2c bus
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }

  // If the RTC unit has lost power, reset the time to the time that the program was compiled. can be used for setting time.
  if (rtc.lostPower()) {
    Serial.println("RTC lost power, lets set the time!");
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
  //rtc.adjust(DateTime(2018,1,27,0,0,30));

  // start the strip up blank
  strip.begin();
  strip.show();
}

void loop() {
  // get the current time
  DateTime now = rtc.now();

  //Clear the strip
  clearStrip();

  //Paint the markers for full hours 0 to 23
  for(int i = 0;i < strip.numPixels(); i++){
    if(i%6==0){
      strip.setPixelColor(i,strip.Color(128,0,0));
    }
  }

  //Paint current hour green
  strip.setPixelColor(now.hour()*6,strip.Color(0,255,0));

  //Paint current minute blue
  int minutePixel = (int)((now.minute()*60+now.second())/25+0.5);
  strip.setPixelColor(minutePixel,strip.Color(0,0,255));

  strip.show();

  delay(500);
}

void clearStrip(){
  for(int i = 0;i < strip.numPixels(); i++){
    strip.setPixelColor(i,strip.Color(0,0,0));
  }
}


