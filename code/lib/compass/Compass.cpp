#include "Compass.h"
#include "EEPROM.h"

void Compass::_eeprom_put() {
   EEPROM.put(0, _x_gain_error);
   EEPROM.put(4, _y_gain_error);
   EEPROM.put(8, _z_gain_error);
   EEPROM.put(12, _x_offset);
   EEPROM.put(16, _y_offset);
   EEPROM.put(20, _z_offset);
}

void Compass::_eeprom_get() {

   EEPROM.get(0, _x_gain_error);
   EEPROM.get(4, _y_gain_error);
   EEPROM.get(8, _z_gain_error);
   if (isnan(_x_gain_error)) _x_gain_error = 1;
   if (isnan(_y_gain_error)) _y_gain_error = 1;
   if (isnan(_z_gain_error)) _z_gain_error = 1;
   #if HMC5883L_DEBUG == 1
      Serial.print("Gain Errors (x, y, z): ");
      Serial.print(_x_gain_error);
      Serial.print(", ");
      Serial.print(_y_gain_error);
      Serial.print(", ");
      Serial.println(_z_gain_error);
   #endif

   EEPROM.get(12, _x_offset);
   EEPROM.get(16, _y_offset);
   EEPROM.get(20, _z_offset);
   if (isnan(_x_offset)) _x_offset = 0;
   if (isnan(_y_offset)) _y_offset = 0;
   if (isnan(_z_offset)) _z_offset = 0;
   #if HMC5883L_DEBUG == 1
      Serial.print("Gain Offset (x, y, z): ");
      Serial.print(_x_offset);
      Serial.print(", ");
      Serial.print(_y_offset);
      Serial.print(", ");
      Serial.println(_z_offset);
   #endif

}

void Compass::initialize() {
   HMC5883L::initialize();
   _gain_factor = _gain_factors[HMC5883L::getGain()];
   #if HMC5883L_DEBUG == 1
      Serial.print("Gain factor: ");
      Serial.println(_gain_factor);
   #endif
   _eeprom_get();
}

void Compass::getHeading(int *x, int *y, int *z) {

   HMC5883L::getHeading(x, y, z);

   *x = *x * _gain_factor * _x_gain_error + _x_offset;
   *y = *y * _gain_factor * _y_gain_error + _y_offset;
   *z = *z * _gain_factor * _z_gain_error + _z_offset;

   #if HMC5883L_DEBUG == 1
      Serial.print("Heading (x, y, z): ");
      Serial.print(*x);
      Serial.print(", ");
      Serial.print(*y);
      Serial.print(", ");
      Serial.println(*z);
   #endif


}

unsigned int Compass::getBearing() {

   unsigned int bearing;
   int x, y, z;
   getHeading(&x, &y, &z);

   float tmp = atan2(y, x);
   if (tmp < 0) tmp += (2 * M_PI);
   bearing = (int) (tmp * 180 / M_PI);
   return bearing;

}

void Compass::calibrate(byte mode) {

   int x, y, z;

   #if HMC5883L_DEBUG == 1
      Serial.println("Calibrating Magnetometer");
   #endif

   if (mode & HMC5883L_CALIBRATE_GAIN) {

      // Positive bias
      HMC5883L::setSampleAveraging(HMC5883L_AVERAGING_8);
      HMC5883L::setDataRate(HMC5883L_RATE_15);
      HMC5883L::setMeasurementBias(HMC5883L_BIAS_POSITIVE);
      HMC5883L::getHeading(&x, &y, &z);
      while ((x<200) | (y<200) | (z<200)) HMC5883L::getHeading(&x, &y, &z);
      x *= _gain_factor;
      y *= _gain_factor;
      z *= _gain_factor;
      _x_gain_error = (float) HMC5883L_COMPASS_XY_EXCITATION / x;
      _y_gain_error = (float) HMC5883L_COMPASS_XY_EXCITATION / y;
      _z_gain_error = (float) HMC5883L_COMPASS_Z_EXCITATION / z;

      // Negative bias
      HMC5883L::setMeasurementBias(HMC5883L_BIAS_NEGATIVE);
      HMC5883L::getHeading(&x, &y, &z);
      while ((x>-200) | (y>-200) | (z>-200)) HMC5883L::getHeading(&x, &y, &z);
      x *= _gain_factor;
      y *= _gain_factor;
      z *= _gain_factor;
      _x_gain_error = ((float) HMC5883L_COMPASS_XY_EXCITATION / abs(x) + _x_gain_error) / 2;
      _y_gain_error = ((float) HMC5883L_COMPASS_XY_EXCITATION / abs(y) + _y_gain_error) / 2;
      _z_gain_error = ((float) HMC5883L_COMPASS_Z_EXCITATION / abs(z) + _z_gain_error) / 2;

      // Recover initial configuration
      HMC5883L::setMeasurementBias(HMC5883L_BIAS_NORMAL);

      #if HMC5883L_DEBUG == 1
         Serial.print("Gain (x, y, z): ");
         Serial.print(_x_gain_error);
         Serial.print(", ");
         Serial.print(_y_gain_error);
         Serial.print(", ");
         Serial.println(_z_gain_error);
      #endif

   }

   if (mode & HMC5883L_CALIBRATE_OFFSET) {

      #if HMC5883L_DEBUG == 1
         Serial.println("Rotate magnetometer 2 or 3 times within 1 minute....");
      #endif

      for (byte i=0; i<10; i++) HMC5883L::getHeading(&x, &y, &z);
      float x_max = -4000;
      float y_max = -4000;
      float z_max = -4000;
      float x_min = 4000;
      float y_min = 4000;
      float z_min = 4000;
      _x_offset = _y_offset = _z_offset = 0;

      unsigned long timeout = millis() + 60000;
      while (millis() < timeout) {
         HMC5883L::getHeading(&x, &y, &z);
         x_max = max(x_max, x);
         y_max = max(y_max, y);
         z_max = max(z_max, z);
         x_min = min(x_min, x);
         y_min = min(y_min, y);
         z_min = min(z_min, z);
      }

      _x_offset = (( x_max - x_min) / 2) - x_max;
      _y_offset = (( y_max - y_min) / 2) - y_max;
      _z_offset = (( z_max - z_min) / 2) - z_max;

      #if HMC5883L_DEBUG == 1
         Serial.print("Offset in mG (x, y, z): ");
         Serial.print(_x_offset);
         Serial.print(", ");
         Serial.print(_y_offset);
         Serial.print(", ");
         Serial.println(_z_offset);
      #endif

   }

   _eeprom_put();

}
