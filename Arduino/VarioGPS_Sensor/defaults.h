/*
  -----------------------------------------------------------
            Settings & defaults
  -----------------------------------------------------------
*/

// **** Sensor defaults ****
#define DEFAULT_UNIT              EU

#define DEFAULT_GPS_MODE          GPS_disabled        //GPS_disabled, GPS_basic, GPS_extended
#define DEFAULT_GPS_3D_DISTANCE   true

#define DEFAULT_MODE_ANALOG_1     analog_disabled     //analog_disabled, analog_enabled
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
  ID_V1,  ID_V2, ID_V3,  ID_V4
};

// Sensor names and unit[EU]
JETISENSOR_CONST sensorsEU[] PROGMEM =
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
  { 0 }
};

// Sensor names and unit[US]
JETISENSOR_CONST sensorsUS[] PROGMEM =
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
  { 0 }
};


// **** Vario settings ****

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


// **** Voltage settings ****

// reference voltage for the ADC
#define V_REF   3.3

// number of analog inputs
#define MAX_ANALOG_INPUTS     4
                                         //  Analog1     Analog2     Analog3     Analog4
const uint8_t analogInputPin[] PROGMEM = {        0,          1,          2,          3  };

const uint8_t analogInputR1[] PROGMEM = {     20000,      20000,      20000,      20000  };
const uint8_t analogInputR2[] PROGMEM = {     10000,      10000,      10000,      10000  };

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


