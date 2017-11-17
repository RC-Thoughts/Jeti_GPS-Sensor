/*
  -----------------------------------------------------------
            Settings & defaults
  -----------------------------------------------------------
*/

// **** Sensor defaults ****

//#define UNIT_US                                     //uncomment for US units

#define DEFAULT_GPS_MODE          GPS_disabled        //GPS_disabled, GPS_basic, GPS_extended
#define DEFAULT_GPS_3D_DISTANCE   true


// Analog input modes
enum {
  analog_disabled,
  voltage,
  ACS758_50B,
  ACS758_100B,
  ACS758_150B,
  ACS758_200B,
  ACS758_50U,
  ACS758_100U,
  ACS758_150U,
  ACS758_200U
};

#define DEFAULT_MODE_ANALOG_1     analog_disabled     
#define DEFAULT_MODE_ANALOG_2     analog_disabled
#define DEFAULT_MODE_ANALOG_3     analog_disabled
#define DEFAULT_MODE_ANALOG_4     analog_disabled


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
  ID_HEADING,
  ID_COURSE,
  ID_SATS,
  ID_HDOP,
  ID_PRESSURE,
  ID_TEMPERATURE,
  ID_HUMIDITY,
  ID_V1, ID_V2, ID_V3, ID_V4,
  ID_A1, ID_A2, ID_A3, ID_A4
};

// Sensor names and unit[EU]
#ifndef UNIT_US
JETISENSOR_CONST sensors[] PROGMEM =
{
  // id             name          unit          data type           precision
  { ID_GPSLAT,      "Latitude",   " ",          JetiSensor::TYPE_GPS, 0 },
  { ID_GPSLON,      "Longitude",  " ",          JetiSensor::TYPE_GPS, 0 },
  { ID_GPSSPEED,    "Speed",      "km/h",       JetiSensor::TYPE_14b, 0 },
  { ID_ALTREL,      "Rel. Altit", "m",          JetiSensor::TYPE_14b, 1 },
  { ID_ALTABS,      "Altitude",   "m",          JetiSensor::TYPE_14b, 0 },
  { ID_VARIO,       "Vario",      "m/s",        JetiSensor::TYPE_30b, 2 },
  { ID_DIST,        "Distance",   "m",          JetiSensor::TYPE_14b, 0 },
  { ID_HEADING,     "Heading",    "\xB0",       JetiSensor::TYPE_14b, 0 },
  { ID_COURSE,      "Course",     "\xB0",       JetiSensor::TYPE_14b, 0 },
  { ID_SATS,        "Satellites", " ",          JetiSensor::TYPE_6b,  0 },
  { ID_HDOP,        "HDOP",       " ",          JetiSensor::TYPE_14b, 2 },
  { ID_PRESSURE,    "Pressure",   "hPa",        JetiSensor::TYPE_30b, 2 },
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
  { ID_ALTREL,      "Rel. Altit", "ft",         JetiSensor::TYPE_14b, 1 },
  { ID_ALTABS,      "Altitude",   "ft",         JetiSensor::TYPE_14b, 0 },
  { ID_VARIO,       "Vario",      "ft/s",       JetiSensor::TYPE_30b, 2 }, 
  { ID_DIST,        "Distance",   "ft.",        JetiSensor::TYPE_14b, 0 },
  { ID_HEADING,     "Heading",    "\xB0",       JetiSensor::TYPE_14b, 0 },
  { ID_COURSE,      "Course",     "\xB0",       JetiSensor::TYPE_14b, 0 },
  { ID_SATS,        "Satellites", " ",          JetiSensor::TYPE_6b,  0 },
  { ID_HDOP,        "HDOP",       " ",          JetiSensor::TYPE_14b, 2 },
  { ID_PRESSURE,    "Pressure",   "inHG",       JetiSensor::TYPE_30b, 2 },
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
  { 0 }
};
#endif


// **** Vario settings ****

// Pressure Sensors
enum {
  unknown,
  BMP280,
  BME280,
  MS5611,
  LPS
};

// Vario lowpass filter and
// dead zone filter in centimeter (Even if you use US-units!)

// BMP280/BME280
#define BMx280_FILTER_X 0.93
#define BMx280_FILTER_Y 0.11
#define BMx280_DEADZONE 5

// MS5611
#define MS5611_FILTER_X 0.8
#define MS5611_FILTER_Y 0.2
#define MS5611_DEADZONE 0

// LPS (LPS311)
#define LPS_FILTER_X 0.8
#define LPS_FILTER_Y 0.2
#define LPS_DEADZONE 0


// **** Analog inputs settings ****

// reference voltage for the ADC
#define V_REF               3300

// number of analog inputs
#define MAX_ANALOG_INPUTS   4
                                         //  Analog1     Analog2     Analog3     Analog4
const uint8_t analogInputPin[] PROGMEM = {        0,          1,          2,          3  };   //Analog Pin



// **** Voltage measurement settings ****

                               //  Analog1      Analog2     Analog3     Analog4
const uint16_t analogInputR1[] {   20000,       20000,      20000,      20000  };   //Resistor R1 in Ohms
const uint16_t analogInputR2[] {   10000,       10000,      10000,      10000  };   //Resistor R2 in Ohms

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

const uint16_t ACS_B_offset = 1650; //bi-directional offset in mV
const uint16_t ACS_U_offset = 396;  //uni-directional offset in mV

// ACS Sensor                    //  ACS758-50B   ACS758-100B   ACS758-150B   ACS758-200B   ACS758-50U    ACS758-100U   ACS758-150U   ACS758-200U
const uint8_t ACS_mVperAmp[] =  {    40,          20,           13,           10,           60,           40,           27,           20          };   //mV per Amp



