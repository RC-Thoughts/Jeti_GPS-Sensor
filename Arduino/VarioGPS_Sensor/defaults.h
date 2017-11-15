/*
  -----------------------------------------------------------
            Settings & defaults
  -----------------------------------------------------------
*/

// **** Sensor defaults ****
#define DEFAULT_UNIT              EU

#define DEFAULT_GPS_MODE          disabled   //disabled, basic, extended
#define DEFAULT_GPS_3D_DISTANCE   true

#define DEFAULT_VOLT1_ENABLE      false
#define DEFAULT_VOLT2_ENABLE      false

// Sensor IDs
enum
{
  ID_GPSLAT       = 1,
  ID_GPSLON       = 2,
  ID_GPSSPEED     = 3,
  ID_ALTREL       = 4,
  ID_ALTABS       = 5,
  ID_VARIO        = 6,
  ID_DIST         = 7,
  ID_HEADING      = 8,
  ID_COURSE       = 9,
  ID_SATS         = 10,
  ID_HDOP         = 11,
  ID_PRESSURE     = 12,
  ID_TEMPERATURE  = 13,
  ID_HUMIDITY     = 14,
  ID_V1           = 15,
  ID_V2           = 16
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
  { 0 }
};



// **** Vario settings ****

// Vario lowpass filter and
// dead zone filter in centimeter (Even if you use US-units!)

// BMP085/BMP180
#define BMP085_FILTER_X 0.5
#define BMP085_FILTER_Y 0.5
#define BMP085_DEADZONE 0

// BMP280/BMP280
#define BMx280_FILTER_X 0.88
#define BMx280_FILTER_Y 0.15
#define BMx280_DEADZONE 3


// **** Voltage settings ****

// reference voltage for the ADC
#define V_REF   3.3

// Analog Pin
#define PIN_V1  0
#define PIN_V2  1

// voltage divider resistors in Ohms
#define VOLTAGE1_R1   20000
#define VOLTAGE1_R2   10000

#define VOLTAGE2_R1   20000
#define VOLTAGE2_R2   10000

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


