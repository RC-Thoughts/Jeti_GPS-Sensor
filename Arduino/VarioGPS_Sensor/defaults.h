/*
  -----------------------------------------------------------
            Settings & defaults
  -----------------------------------------------------------
*/

// **** Supported features & options ****

//#define UNIT_US                             //uncomment to enable US units

#define V_REF                     3300        // set supply voltage from 1800 to 5500mV
       
#define SUPPORT_BMx280     
#define SUPPORT_MS5611_LPS  
#define SUPPORT_GPS

// **************************************



// Sensor IDs
enum
{
  ID_GPSLAT = 1,
  ID_GPSLON,
  ID_GPSSPEED,
  ID_ALTREL,
  ID_ALTABS,
  ID_VARIO,
  ID_DIST,
  ID_TRIP,
  ID_HEADING,
  ID_COURSE,
  ID_SATS,
  ID_HDOP,
  ID_PRESSURE,
  ID_TEMPERATURE,
  ID_HUMIDITY,
  ID_V1, ID_V2, ID_V3, ID_V4,
  ID_A1, ID_A2, ID_A3, ID_A4,
  ID_C1, ID_C2, ID_C3, ID_C4
};

/*
TYPE_6b   int6_t    Data type 6b (-31 to 31)
TYPE_14b  int14_t   Data type 14b (-8191 to 8191)
TYPE_22b  int22_t   Data type 22b (-2097151 to 2097151)
TYPE_DT   int22_t   Special data type for time and date
TYPE_30b  int30_t   Data type 30b (-536870911 to 536870911) 
TYPE_GPS  int30_t   Special data type for GPS coordinates:  lo/hi minute - lo/hi degree.
*/    

// Sensor names and unit[EU]
#ifndef UNIT_US
JETISENSOR_CONST sensors[] PROGMEM =
{
  // id             name          unit          data type           precision
  { ID_GPSLAT,      "Latitude",   " ",          JetiSensor::TYPE_GPS, 0 },
  { ID_GPSLON,      "Longitude",  " ",          JetiSensor::TYPE_GPS, 0 },
  { ID_GPSSPEED,    "Speed",      "km/h",       JetiSensor::TYPE_14b, 0 },
  { ID_ALTREL,      "Rel. Altit", "m",          JetiSensor::TYPE_22b, 1 },
  { ID_ALTABS,      "Altitude",   "m",          JetiSensor::TYPE_22b, 0 },
  { ID_VARIO,       "Vario",      "m/s",        JetiSensor::TYPE_22b, 2 },
  { ID_DIST,        "Distance",   "m",          JetiSensor::TYPE_22b, 0 },
  { ID_TRIP,        "Trip",       "km",         JetiSensor::TYPE_22b, 2 },
  { ID_HEADING,     "Heading",    "\xB0",       JetiSensor::TYPE_14b, 0 },
  { ID_COURSE,      "Course",     "\xB0",       JetiSensor::TYPE_14b, 0 },
  { ID_SATS,        "Satellites", " ",          JetiSensor::TYPE_6b,  0 },
  { ID_HDOP,        "HDOP",       " ",          JetiSensor::TYPE_14b, 2 },
  { ID_PRESSURE,    "Pressure",   "hPa",        JetiSensor::TYPE_22b, 2 },
  { ID_TEMPERATURE, "Temperature","\xB0\x43",   JetiSensor::TYPE_14b, 1 },
  { ID_HUMIDITY,    "Humidity",   "%rH",        JetiSensor::TYPE_14b, 1 },
  { ID_V1,          "Voltage1",   "V",          JetiSensor::TYPE_14b, 2 },
  { ID_V2,          "Voltage2",   "V",          JetiSensor::TYPE_14b, 2 },
  { ID_V3,          "Voltage3",   "V",          JetiSensor::TYPE_14b, 2 },
  { ID_V4,          "Voltage4",   "V",          JetiSensor::TYPE_14b, 2 },
  { ID_A1,          "Current1",   "A",          JetiSensor::TYPE_14b, 1 },
  { ID_A2,          "Current2",   "A",          JetiSensor::TYPE_14b, 1 },
  { ID_A3,          "Current3",   "A",          JetiSensor::TYPE_14b, 1 },
  { ID_A4,          "Current4",   "A",          JetiSensor::TYPE_14b, 1 },
  { ID_C1,          "Capacity1",  "mAh",        JetiSensor::TYPE_22b, 0 },
  { ID_C2,          "Capacity2",  "mAh",        JetiSensor::TYPE_22b, 0 },
  { ID_C3,          "Capacity3",  "mAh",        JetiSensor::TYPE_22b, 0 },
  { ID_C4,          "Capacity4",  "mAh",        JetiSensor::TYPE_22b, 0 },
  { 0 }
};
#endif


// Sensor names and unit[US]
#ifdef UNIT_US
JETISENSOR_CONST sensors[] PROGMEM =
{
  // id             name          unit          data type           precision
  { ID_GPSLAT,      "Latitude",   " ",          JetiSensor::TYPE_GPS, 0 },
  { ID_GPSLON,      "Longitude",  " ",          JetiSensor::TYPE_GPS, 0 },
  { ID_GPSSPEED,    "Speed",      "mph",        JetiSensor::TYPE_14b, 0 },
  { ID_ALTREL,      "Rel. Altit", "ft",         JetiSensor::TYPE_22b, 1 },
  { ID_ALTABS,      "Altitude",   "ft",         JetiSensor::TYPE_22b, 0 },
  { ID_VARIO,       "Vario",      "ft/s",       JetiSensor::TYPE_22b, 2 }, 
  { ID_DIST,        "Distance",   "ft.",        JetiSensor::TYPE_22b, 0 },
  { ID_TRIP,        "Trip",       "mi",         JetiSensor::TYPE_22b, 2 },
  { ID_HEADING,     "Heading",    "\xB0",       JetiSensor::TYPE_14b, 0 },
  { ID_COURSE,      "Course",     "\xB0",       JetiSensor::TYPE_14b, 0 },
  { ID_SATS,        "Satellites", " ",          JetiSensor::TYPE_6b,  0 },
  { ID_HDOP,        "HDOP",       " ",          JetiSensor::TYPE_14b, 2 },
  { ID_PRESSURE,    "Pressure",   "inHG",       JetiSensor::TYPE_22b, 2 },
  { ID_TEMPERATURE, "Temperature","\xB0\x46",   JetiSensor::TYPE_14b, 1 },
  { ID_HUMIDITY,    "Humidity",   "%rH",        JetiSensor::TYPE_14b, 1 },
  { ID_V1,          "Voltage1",   "V",          JetiSensor::TYPE_14b, 2 },
  { ID_V2,          "Voltage2",   "V",          JetiSensor::TYPE_14b, 2 },
  { ID_V3,          "Voltage3",   "V",          JetiSensor::TYPE_14b, 2 },
  { ID_V4,          "Voltage4",   "V",          JetiSensor::TYPE_14b, 2 },
  { ID_A1,          "Current1",   "A",          JetiSensor::TYPE_14b, 1 },
  { ID_A2,          "Current2",   "A",          JetiSensor::TYPE_14b, 1 },
  { ID_A3,          "Current3",   "A",          JetiSensor::TYPE_14b, 1 },
  { ID_A4,          "Current4",   "A",          JetiSensor::TYPE_14b, 1 },
  { ID_C1,          "Capacity1",  "mAh",        JetiSensor::TYPE_22b, 0 },
  { ID_C2,          "Capacity2",  "mAh",        JetiSensor::TYPE_22b, 0 },
  { ID_C3,          "Capacity3",  "mAh",        JetiSensor::TYPE_22b, 0 },
  { ID_C4,          "Capacity4",  "mAh",        JetiSensor::TYPE_22b, 0 },
  { 0 }
};
#endif


// **** General settings ****

#define MEASURING_INTERVAL        180         //ms



// **** Vario settings ****

// Pressure Sensors
enum {
  unknown,
  BMP280,
  BME280,
  MS5611_,
  LPS_
};

// Vario lowpass filter and
// dead zone filter in centimeter (Even if you use US-units!)

// BMP280/BME280
#define BMx280_FILTER_X 0.88 //0.93
#define BMx280_FILTER_Y 0.15 //0.11
#define BMx280_DEADZONE 5

// MS5611
#define MS5611_FILTER_X 0.8
#define MS5611_FILTER_Y 0.2
#define MS5611_DEADZONE 0

// LPS (LPS311)
#define LPS_FILTER_X 0.8
#define LPS_FILTER_Y 0.2
#define LPS_DEADZONE 0


// **** GPS settings ****

#define GPSBaud 9600

// GPS
enum {
  GPS_disabled,
  GPS_basic,
  GPS_extended
};



// **** Analog inputs settings ****

#if V_REF < 1800 || V_REF > 5500
  #error unsupported supply voltage
#endif
  

// suported analog input modes 
enum {
  analog_disabled,
  voltage,
  #if V_REF >= 4500
  ACS712_05,
  ACS712_20,
  ACS712_30,
  #endif
  ACS758_50B,
  ACS758_100B,
  ACS758_150B,
  ACS758_200B,
  ACS758_50U,
  ACS758_100U,
  ACS758_150U,
  ACS758_200U
};


// number of analog inputs
#define MAX_ANALOG_INPUTS   4
                                //  Analog1     Analog2     Analog3     Analog4
const uint8_t analogInputPin[] = {        0,          1,          2,          3  };   //Analog Pin



// **** Voltage measurement settings ****

                                 //  Analog1      Analog2     Analog3     Analog4
const uint16_t analogInputR1[] = {   20000,       20000,      20000,      20000  };   //Resistor R1 in Ohms
const uint16_t analogInputR2[] = {   10000,       10000,      10000,      10000  };   //Resistor R2 in Ohms

/*
                  voltage input
                     |
                     |
                    | |
                    | |  R1
                    | |
                     |
  analog Pin  <------+
                     |
                    | |
                    | |  R2
                    | |
                     |
                     |
                    GND
*/



// **** Current measurement settings ****

const uint16_t ACS_B_offset = V_REF/2; //bi-directional offset in mV ( V_REF / 2)
const uint16_t ACS_U_offset = V_REF/8.33;  //uni-directional offset in mV ( V_REF / 8.33)

                              //mV per Amp
const uint8_t ACS_mVperAmp[] =  { 
                                  #if V_REF >= 4500
                                  185,      // ACS712-05
                                  100,      // ACS712-20
                                   66,      // ACS712-30
                                  #endif
                                   40,      // ACS758-50B
                                   20,      // ACS758-100B
                                   13,      // ACS758-150B
                                   10,      // ACS758-200B 
                                   60,      // ACS758-50U
                                   40,      // ACS758-100U
                                   27,      // ACS758-150U
                                   20 };    // ACS758-200U



// **** Default settings ****

#define DEFAULT_GPS_MODE          GPS_disabled        //GPS_disabled, GPS_basic, GPS_extended
#define DEFAULT_GPS_3D_DISTANCE   true

#define DEFAULT_MODE_ANALOG_1     analog_disabled     
#define DEFAULT_MODE_ANALOG_2     analog_disabled
#define DEFAULT_MODE_ANALOG_3     analog_disabled
#define DEFAULT_MODE_ANALOG_4     analog_disabled





