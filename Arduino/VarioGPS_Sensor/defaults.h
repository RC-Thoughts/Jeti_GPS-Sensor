/*
  -----------------------------------------------------------
            Settings & defaults
  -----------------------------------------------------------
*/

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
  { ID_TRIP,        "Trip",       "km",         JetiSensor::TYPE_14b, 2 },
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
  { ID_ALTREL,      "Rel. Altit", "ft",         JetiSensor::TYPE_14b, 1 },
  { ID_ALTABS,      "Altitude",   "ft",         JetiSensor::TYPE_14b, 0 },
  { ID_VARIO,       "Vario",      "ft/s",       JetiSensor::TYPE_30b, 2 }, 
  { ID_DIST,        "Distance",   "ft.",        JetiSensor::TYPE_14b, 0 },
  { ID_TRIP,        "Trip",       "mi",         JetiSensor::TYPE_14b, 2 },
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
  { ID_C1,          "Capacity1",  "mAh",        JetiSensor::TYPE_22b, 0 },
  { ID_C2,          "Capacity2",  "mAh",        JetiSensor::TYPE_22b, 0 },
  { ID_C3,          "Capacity3",  "mAh",        JetiSensor::TYPE_22b, 0 },
  { ID_C4,          "Capacity4",  "mAh",        JetiSensor::TYPE_22b, 0 },
  { 0 }
};
#endif

// GPS
enum {
  GPS_disabled,
  GPS_basic,
  GPS_extended
};


// **** Supported features ****

//uncomment to enable

//#define UNIT_US
//#define SUPPLY_VOLTAGE_3V3 
#define SUPPLY_VOLTAGE_5V        
#define SUPPORT_BMx280     
#define SUPPORT_MS5611_LPS                               



// **** General settings ****

#define MEASURING_INTERVAL        180                 //ms



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


// **** Analog inputs settings ****

// Supply voltage

#ifdef SUPPLY_VOLTAGE_3V3   
  #define V_REF               3300

  // suported analog input modes @3.3V
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
#endif

#ifdef SUPPLY_VOLTAGE_5V  
  #define V_REF               5000

  // suported analog input modes @5V
  enum {
    analog_disabled,
    voltage,
    ACS712_05,
    ACS712_20,
    ACS712_30,
    ACS758_50B,
    ACS758_100B,
    ACS758_150B,
    ACS758_200B,
    ACS758_50U,
    ACS758_100U,
    ACS758_150U,
    ACS758_200U
  };
#endif

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

#ifdef SUPPLY_VOLTAGE_3V3 
  const uint16_t ACS_B_offset = 1650; //bi-directional offset in mV ( V_REF / 2)
  const uint16_t ACS_U_offset = 396;  //uni-directional offset in mV ( V_REF / 8.33)

  // ACS Sensor                    //  ACS758-50    ACS758-100    ACS758-150    ACS758-200    
  const uint8_t ACS_mVperAmp[] =  {    40,          20,           13,           10,               //bi-directional type, mV per Amp
                                       60,          40,           27,           20          };    //uni-directional type, mV per Amp
#endif

#ifdef SUPPLY_VOLTAGE_5V 
  const uint16_t ACS_B_offset = 2500; //bi-directional offset in mV ( V_REF / 2)
  const uint16_t ACS_U_offset = 600;  //uni-directional offset in mV ( V_REF / 8.33)

  // at 5V supply, additional ACS712 types are supported
  // ACS Sensor                    //  ACS712-05    ACS712-20    ACS712-30     ACS758-50    ACS758-100    ACS758-150    ACS758-200    
  const uint8_t ACS_mVperAmp[] =  {    185,         100,         66,           40,          20,           13,           10,               //bi-directional type, mV per Amp
                                                                               60,          40,           27,           20          };    //uni-directional type, mV per Amp
#endif



// **** Defaults settings ****

#define DEFAULT_GPS_MODE          GPS_disabled        //GPS_disabled, GPS_basic, GPS_extended
#define DEFAULT_GPS_3D_DISTANCE   true

#define DEFAULT_MODE_ANALOG_1     analog_disabled     
#define DEFAULT_MODE_ANALOG_2     analog_disabled
#define DEFAULT_MODE_ANALOG_3     analog_disabled
#define DEFAULT_MODE_ANALOG_4     analog_disabled





