/*

Solr
Open Source digital solar clock
Copyright (C) 2016 by Xose Pérez <xose dot perez at gmail dot com>

--------------------------------------------------------------------------------

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

--------------------------------------------------------------------------------

Pinout for HP Bubble Display (common cathode LEDs):

+--A--B--2--F--D--G--+
| 4 x 7 Segment + DP |
+--1--E--C--3--DP-4--+

MOSI (PIN 15, D11)
cannot be connected to LED display while programming

*/

#include "SevSeg.h"
#include "Compass.h"
#include "LowPower.h"

#define DEBUG 0

#define DIGITS 4
#define BRIGHTNESS 100

#define DIGIT_1 7
#define DIGIT_2 10
#define DIGIT_3 14
#define DIGIT_4 17

#define SEGMENT_A 6
#define SEGMENT_B 8
#define SEGMENT_C 12
#define SEGMENT_D 15
#define SEGMENT_E 9
#define SEGMENT_F 13
#define SEGMENT_G 16

#define DECIMAL_POINT 0 // NC
#define BUTTON_PIN 2
#define UPDATE_INTERVAL 1000
#define MIN_STABLE_COUNT 20

// Seven segments display object
SevSeg sevseg;

// HMC5883L class
Compass compass;

// Current time for display
unsigned long next_update;

void showText(char * text, unsigned long ms = 0) {
    unsigned long timeout = millis() + ms;
    while (millis() < timeout) {
        sevseg.DisplayString(text, 0);
    }
}

void showText(const char * text, unsigned long ms = 0) {
    showText((char *) text, ms);
}

int getBearing() {

    byte count = 0;
    int bearing = 0;

    while (count < MIN_STABLE_COUNT) {
       delay(20);
       int tmp = compass.getBearing();
       if (bearing == tmp) {
          count++;
       } else {
          bearing = tmp;
          count = 0;
       }
    }

    return bearing;

}

void updateDisplay(bool showTime = true) {

    // 0º is south, 90% west
    int bearing = getBearing();
    bearing = (bearing + 180) % 360;

    // 4 minutes per degree
    int minutes = 4 * bearing;
    int quarters = minutes / 15;
    int quarter = quarters % 4;
    int hour = (quarters - quarter) / 4;

    minutes = minutes % 60;
    minutes = (minutes / 5) * 5;
    hour = (hour + 2) % 24;

    char display[5];

    if (showTime) {
        sprintf(display, "%02i%02i", hour, minutes);
    } else {
        sprintf(display, "%03i!", bearing);
    }
    showText(display, 5000);

    #if DEBUG == 1
        Serial.print("Bearing: ");
        Serial.println(bearing);
        Serial.print("Hour: ");
        if (hour < 10) Serial.print("0");
        Serial.print(hour);
        Serial.print(":");
        if (minutes < 10) Serial.print("0");
        Serial.println(minutes);
        Serial.print("Display: ");
        Serial.println(display);
    #endif


}

void checkCalibration() {

   if (digitalRead(BUTTON_PIN) == HIGH) {
      delay(50);
      if (digitalRead(BUTTON_PIN) == HIGH) {

         // start calibration
         showText(" SET", 1000);
         showText("GAIN", 1000);
         compass.calibrate(HMC5883L_CALIBRATE_GAIN);
         showText("DONE", 1000);
         showText("OFFS", 1000);
         compass.calibrate(HMC5883L_CALIBRATE_OFFSET);
         showText("DONE", 1000);

      }
   }

}

void wakeUp() {
   // wake up
}

void setup() {

    // Initialize debug console
    #if DEBUG == 1
        Serial.begin(9600);
        while (!Serial);
    #endif

    // Configure seven segments display
    sevseg.Begin(
        COMMON_CATHODE, DIGITS,
        DIGIT_1, DIGIT_2, DIGIT_3, DIGIT_4,
        SEGMENT_A, SEGMENT_B, SEGMENT_C, SEGMENT_D, SEGMENT_E, SEGMENT_F, SEGMENT_G,
        DECIMAL_POINT
    );
    sevseg.SetBrightness(BRIGHTNESS);

    // Join I2C bus
    Wire.begin();

    // Test connection
    if (!compass.testConnection()) {
        showText("ERR1", 5000);
        while (true);
    }

    // Initialize compass
    compass.initialize();

    // Show welcome text
    showText("SOLR", 2000);

    // Define button interrupt
    pinMode(BUTTON_PIN, INPUT);

    // Check if calibration mode
    checkCalibration();

    // Next update time
    next_update = millis() + UPDATE_INTERVAL;

}

void loop() {

    // Allow wake up pin to trigger interrupt on low.
    attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), wakeUp, HIGH);

    // Enter power down state with ADC and BOD module disabled.
    // Wake up when wake up pin is low.
    LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);

    // Disable external pin interrupt on wake up pin.
    detachInterrupt(digitalPinToInterrupt(BUTTON_PIN));

    // Show
    updateDisplay(true);

}
