/*
  -----------------------------------------------------------
            Settings & defaults
  -----------------------------------------------------------
*/

// **** General settings ****************

//#define UNIT_US                             //uncomment to enable US units

#define V_REF                     3300        // set supply voltage from 1800 to 5500mV

// supported devices
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
  ID_VOLTAGE,
  ID_CURRENT,
  ID_CAPACITY,
  ID_POWER,
  ID_RX1_VOLTAGE,
  ID_RX2_VOLTAGE,
  ID_EXT_TEMP
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
  { ID_VOLTAGE,     "Voltage",    "V",          JetiSensor::TYPE_14b, 1 },
  { ID_CURRENT,     "Current",    "A",          JetiSensor::TYPE_14b, 1 },
  { ID_CAPACITY,    "Capacity",   "mAh",        JetiSensor::TYPE_22b, 0 },
  { ID_POWER,       "Power",      "W",          JetiSensor::TYPE_22b, 0 },
  { ID_RX1_VOLTAGE, "Rx1 Voltage","V",          JetiSensor::TYPE_14b, 2 },
  { ID_RX2_VOLTAGE, "Rx2 Voltage","V",          JetiSensor::TYPE_14b, 2 },
  { ID_EXT_TEMP,    "Ext. Temp",  "\xB0\x43",   JetiSensor::TYPE_14b, 1 },
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
  { ID_VOLTAGE,     "Voltage",    "V",          JetiSensor::TYPE_14b, 1 },
  { ID_CURRENT,     "Current",    "A",          JetiSensor::TYPE_14b, 1 },
  { ID_CAPACITY,    "Capacity",   "mAh",        JetiSensor::TYPE_22b, 0 },
  { ID_POWER,       "Power",      "W",          JetiSensor::TYPE_22b, 0 },
  { ID_RX1_VOLTAGE, "Rx1 Voltage","V",          JetiSensor::TYPE_14b, 2 },
  { ID_RX2_VOLTAGE, "Rx2 Voltage","V",          JetiSensor::TYPE_14b, 2 },
  { ID_EXT_TEMP,    "Ext. Temp",  "\xB0\x46",   JetiSensor::TYPE_14b, 1 },
  { 0 }
};
#endif



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
#define BMx280_FILTER_X 0.88
#define BMx280_FILTER_Y 0.15 
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

// GPS mode
enum {
  GPS_disabled,
  GPS_basic,
  GPS_extended
};



// **** Voltage measurement settings ****

// analog input pin
#define VOLTAGE_PIN         A1
#define RX1_VOLTAGE_PIN     A2
#define RX2_VOLTAGE_PIN     A3


// suported voltage sensors 
enum {
  AttoPilot_45V,
  AttoPilot_90V,
  AttoPilot_180V,
  APM25_V,
  ACS712_voltage,
  ACS758_voltage,
  Rx1_voltage,
  Rx2_voltage
};


//                                  AttoPilot_45    AttoPilot_90    AttoPilot_180     APM25           ACS712          ACS758        Rx1 Voltage     Rx2 Voltage
// max. voltage @3.3V vref             13.6V           51.8V           51.8V           33.4V           36.3V           62.7V            9.9V            9.9V
const uint16_t voltageInputR1[] = {   14700,          14700,          14700,          13700,          10000,          18000,          20000,          20000,  };   //Resistor R1 in Ohms
const uint16_t voltageInputR2[] = {    4700,           1000,           1000,           1500,           1000,           1000,          10000,          10000,  };   //Resistor R2 in Ohms

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

// analog input pin
#define CURRENT_PIN     A0

// capacity reset mode
enum {
  startup,
  automatic,
  manual
};

// save capacity in automatic mode
#define CAPACITY_SAVE_INTERVAL        10000         // ms
#define MAX_CUR_TO_SAVE_CAPACITY      2             // A

// voltage difference to reset
#define VOLTAGE_DIFFERENCE            2             // %  

// suported current sensors 
enum {
  mainDrive_disabled,
  AttoPilot_45A,
  AttoPilot_90A,
  AttoPilot_180A,
  APM25_A,
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

// Offset for AttoPilot and APM sensor
const uint16_t Atto_APM_offset = 0;

// Offset for ACS sensor
const uint16_t ACS_B_offset = V_REF/2; //bi-directional offset in mV ( V_REF / 2)
const uint16_t ACS_U_offset = V_REF/8.33;  //uni-directional offset in mV ( V_REF / 8.33)

                         //   mV/Amp @5V            sensor type
const uint8_t mVperAmp[] =  { 
                              73,               // AttoPilot 45A
                              37,               // AttoPilot 90A
                              18,               // AttoPilot 180A
                              73,               // APM 2.5 90A
                              #if V_REF >= 4500
                              185,              // ACS712-05
                              100,              // ACS712-20
                               66,              // ACS712-30
                              #endif
                               40,              // ACS758-50B
                               20,              // ACS758-100B
                               13,              // ACS758-150B
                               10,              // ACS758-200B 
                               60,              // ACS758-50U
                               40,              // ACS758-100U
                               27,              // ACS758-150U
                               20               // ACS758-200U
                             };



// **** Temperature measurement settings ****

// analog input pin
#define TEMPERATURE_PIN     A6

// Thermistor nominal resistance at 25ºC
#define THERMISTORNOMINAL   10000

// temp. for nominal resistance (almost always 25ºC)
#define TEMPERATURENOMINAL  25

// thermistor's beta coefficient
#define BCOEFFICIENT        3950

// Self-heat compensation (K)
#define SELF_HEAT           1.2

// Value of the series resistor
#define SERIESRESISTOR      10000

/*
                    vcc
                     |
                     |
                    | |
                    | |  Resistor
                    | |
                     |
  analog Pin  <------+
                     |
                    | |
                    | |  NTC
                    | |
                     |
                     |
                    GND
*/




// **** Default settings ****

#define DEFAULT_GPS_MODE          GPS_disabled          //GPS_disabled, GPS_basic, GPS_extended
#define DEFAULT_GPS_3D_DISTANCE   true

#define DEFAULT_CURRENT_SENSOR    mainDrive_disabled
#define DEFAULT_CAPACITY_MODE     automatic             //startup, automatic, manual

#define DEFAULT_ENABLE_Rx1        false
#define DEFAULT_ENABLE_Rx2        false

#define DEFAULT_ENABLE_EXT_TEMP   false


