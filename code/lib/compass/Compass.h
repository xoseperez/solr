#ifndef COMPASS_H

   #include "Wire.h"
   #include "I2Cdev.h"
   #include "HMC5883L.h"
   #include "EEPROM.h"

   #define COMPASS_H

   #define HMC5883L_DEBUG                       0x01
   #define HMC5883L_CALIBRATE_GAIN              0x01
   #define HMC5883L_CALIBRATE_OFFSET            0x02

   #define HMC5883L_COMPASS_XY_EXCITATION       1160
   #define HMC5883L_COMPASS_Z_EXCITATION        1080

   class Compass : public HMC5883L {

      private:

         float _x_gain_error;
         float _y_gain_error;
         float _z_gain_error;
         float _x_offset;
         float _y_offset;
         float _z_offset;
         float _gain_factor;
         float _gain_factors[8] = {0.73, 0.92, 1.22, 1.52, 2.27, 2.56, 3.03, 4.35};

         void _eeprom_put();
         void _eeprom_get();

      public:
         void initialize();
         void calibrate(byte mode);
         void getHeading(int *x, int *y, int *z);
         unsigned int getBearing();

   };

#endif
