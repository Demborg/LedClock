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

// Connect the HC-06 TX to the Arduino RX on pin 2.
// Connect the HC-06 RX to the Arduino TX on pin 3 through a voltage divider.
#include <SoftwareSerial.h>
SoftwareSerial BTserial(2, 3); // RX | TX


//Variable for indicating state of the system
char state = 'c'; //c means clock
int wHour = -1; //hour to wake
int wMinute = -1; //minute to wake

//Variables for colors
uint32_t bgColor = strip.Color(0,0,0); //bg or solid/strobe color
uint32_t eColor = strip.Color(32,0,0); //colors for even ticks, odd ticks, minutes and hours
uint32_t oColor = strip.Color(32,0,0);
uint32_t mColor = strip.Color(0,0,255);
uint32_t hColor = strip.Color(0,255,0);
uint32_t wColor = strip.Color(255,255,0); //wake up indicator

void setup() {
  Serial.begin(9600);

  // HC-06 default serial speed is 9600
  BTserial.begin(9600);

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

  //Clear the strip
  fillStrip(bgColor);

  if (BTserial.available()) {
    while (BTserial.available()) {
      char ch = BTserial.read();
      if(ch == '?'){
        BTserial.println("Welcome! Letters correspond to differnt lighting options");
        BTserial.println("'c' = clock");
        BTserial.println("'b' = blink");
        BTserial.println("'s' = solid");
        BTserial.println("The clock can wake you upp at a given time");
        BTserial.println("'ahhmm' sets the alarm for hh:mm and starts turning the lights on 30 minutes before. 'a....' turns the alarm function off");
        BTserial.println("The colors of the clock can be fully customized to your liking");
        BTserial.println("'gxxx' sets the background color where x is a number 0-9 and corresponds to the intensity of r, g and b");
        BTserial.println("'exxx' sets the color of even ticks");
        BTserial.println("'oxxx' sets the color of odd ticks");
        BTserial.println("'mxxx' sets the color of minute");
        BTserial.println("'mxxx' sets the color of hour");
        BTserial.println("'wxxx' sets the color of wake indicator");
      }
      else if(ch == 'g'){
        bgColor = getColor();
        BTserial.println("background color set");
      }
      else if(ch == 'e'){
        eColor = getColor();
        BTserial.println("even tick color set");
      }
      else if(ch == 'o'){
        oColor = getColor();
        BTserial.println("odd tick color set");
      }
      else if(ch == 'm'){
        mColor = getColor();
        BTserial.println("minute color set");
      }
      else if(ch == 'h'){
        hColor = getColor();
        BTserial.println("hour color set");
      }
      else if(ch == 'w'){
        wColor = getColor();
        BTserial.println("wake color set");
      }
      else if(ch == 'a'){
        wHour = (BTserial.read()-'0')*10 + BTserial.read()-'0';
        wMinute = (BTserial.read()-'0')*10 + BTserial.read()-'0';
        BTserial.println("alarm set");
      }
      else{
        state = ch;
        BTserial.println("State changed");
      }
      Serial.println("Here comes some debug stuff");
      Serial.println(ch);
      Serial.println(state);
    }
    //Stuff for alarm light
    DateTime now = rtc.now();
    if(now.hour() == wHour && now.minute() == wMinute){
      bgColor = strip.Color(128,70,20);
    }
    
  }

  if (state == 'c') {
    showTime();
  }
  else if (state == 'b') {
    strobe(bgColor);
  }
  else{
    fillStrip(bgColor);
  }


  strip.show();

  delay(500);
}

void fillStrip(uint32_t color) {
  for (int i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, color);
  }
}

void showTime() {
  // get the current time
  DateTime now = rtc.now();

  //Paint the markers for full hours 0 to 23
  for (int i = 0; i < strip.numPixels(); i++) {
    if (i % 12 == 11) {
      strip.setPixelColor(i, eColor);
    }
    else if (i % 6 == 5) {
      strip.setPixelColor(i, oColor);
    }
  }

  //Paint current hour green
  int hourPixel = (int)((now.hour() * 60 + now.minute()) / 10 + 0.5);
  strip.setPixelColor(hourPixel, hColor);

  //Paint current minute blue
  int minutePixel = (int)((now.minute() * 60 + now.second()) / 25 + 0.5);
  strip.setPixelColor(minutePixel, mColor);

  //If alarm set paint that
  if(wHour >= 0){
    int hourPixel = (int)((wHour * 60 + wMinute) / 10 + 0.5);
    strip.setPixelColor(hourPixel, wColor);
  }
}

void strobe(uint32_t c) {
  fillStrip(c);
  strip.show();
  delay(1);
  fillStrip(strip.Color(0, 0, 0));
}

uint32_t getColor(){
  int r = (BTserial.read() - '0')*28;
  int g = (BTserial.read() - '0')*28;
  int b = (BTserial.read() - '0')*28;

  //Protection against drawing to much power
  if(r+g+b > 400){
    BTserial.println("To high values, protecting");
    r /= (r+g+b)/400.0;
    g /= (r+g+b)/400.0;
    b /= (r+g+b)/400.0;
  }

  return strip.Color(r,g,b);
}

