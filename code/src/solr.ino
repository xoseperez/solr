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
#include "I2Cdev.h"
#include "HMC5883L.h"

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

#define UPDATE_INTERVAL 1000

// Seven segments display object
SevSeg sevseg;

// class default I2C address is 0x1E
// specific I2C addresses may be passed as a parameter here
// this device only supports one I2C address (0x1E)
HMC5883L mag;

// Holding string for display
char display[5];

// Current time for display
unsigned long next_update;

// Holding values for heading
int16_t mx, my, mz;

void setup() {

   // join I2C bus (I2Cdev library doesn't do this automatically)
   Wire.begin();

   // Initialize compass
   mag.initialize();

   // Configure seven segments display
   sevseg.Begin(
      COMMON_CATHODE, DIGITS,
      DIGIT_1, DIGIT_2, DIGIT_3, DIGIT_4,
      SEGMENT_A, SEGMENT_B, SEGMENT_C, SEGMENT_D, SEGMENT_E, SEGMENT_F, SEGMENT_G,
      DECIMAL_POINT
   );
   sevseg.SetBrightness(BRIGHTNESS);

   // Default value
   strncpy(display, "GOOD", 4);

   // Next update time
   next_update = millis() + UPDATE_INTERVAL;

}

void loop() {

   // Update heading
   if (millis() > next_update) {

      // read raw heading measurements from device
      mag.getHeading(&mx, &my, &mz);
      float heading = atan2(my, mx);
      if (heading < 0)
         heading += 2 * M_PI;

      sprintf (display, "%03i!", (int) (heading * 180/M_PI));

      next_update += UPDATE_INTERVAL;

   }

    sevseg.DisplayString(display, 0);

}
