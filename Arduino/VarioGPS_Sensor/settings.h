/*
  -----------------------------------------------------------
            Settings 
  -----------------------------------------------------------
  General settings for the VarioGPS sensor.
*/

//#define UNIT_US                             // uncomment to enable US units

#define V_REF                     5000        // set supply voltage from 1800 to 5500mV


// **** supported devices, comment to disable ****
// Turn on the compiler warnings in the arduino settings to indicate compatibility issues

//#define SUPPORT_EX_BUS                        // enable bidirectional EX Bus with 125kbaud, otherwise the old EX protocol with 9.6kbaud is enabled

#define SUPPORT_BMx280                        // BMP280 and BME280 pressure sensors for altitude
#define SUPPORT_MS5611                        // MS5611 pressure sensor for altitude
//#define SUPPORT_LPS                           // LPSxxx (from STmicroelectronics) pressure sensors for altitude 

#define SUPPORT_GPS                           // GPS modul
#define SUPPORT_MPXV7002_MPXV5004             // MPXV7002 or MPXV5004 pressure sensor for airspeed
#define SUPPORT_TEC                           // Vario TEC compensation

#define SUPPORT_MAIN_DRIVE                    // current and voltage sensors
#define SUPPORT_RX_VOLTAGE                    // Rx voltage sensors

#define SUPPORT_EXT_TEMP                      // NTC temperature sensor

#define SUPPORT_JETIBOX_MENU                  // JetiBox menu

