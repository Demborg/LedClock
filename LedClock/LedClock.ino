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
bool state = true;

//Scaling for brightness
int scale = 1;

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
  fillStrip(strip.Color(0, 0, 0));

  if (BTserial.available()) {
    while (BTserial.available()) {
      char ch = BTserial.read();
      if(ch <= '9' && ch >= '0'){
        scale = 10 - ch + '0' ;
        BTserial.println("Brightness set");
      }
      else{
        state = !state;
        BTserial.println("State toggled");
      }
      Serial.println("Here comes some debug stuff");
      Serial.println(ch);
      Serial.println(state);
      Serial.println(scale);
    }
  }

  if (state) {
    showTime();
  }
  else {
    party();
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
    if (i % 12 == 0) {
      strip.setPixelColor(i, strip.Color(64/scale, 0, 0));
    }
    else if (i % 6 == 0) {
      strip.setPixelColor(i, strip.Color(64/scale, 0, 0));
    }
  }

  //Paint current hour green
  int hourPixel = (int)((now.hour() * 60 + now.minute()) / 10 + 0.5);
  strip.setPixelColor(hourPixel, strip.Color(0, 255/scale, 0));

  //Paint current minute blue
  int minutePixel = (int)((now.minute() * 60 + now.second()) / 25 + 0.5);
  strip.setPixelColor(minutePixel, strip.Color(0, 0, 255/scale));
}

void party() {
  fillStrip(strip.Color(128/scale, 128/scale, 128/scale));
  strip.show();
  delay(50);
  fillStrip(strip.Color(0, 0, 0));
}

