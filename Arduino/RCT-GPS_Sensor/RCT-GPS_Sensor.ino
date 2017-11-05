/*
  -----------------------------------------------------------
            Jeti GPS Sensor V1.6
  -----------------------------------------------------------

   Based on the "Jeti EX MegaSensor for Teensy 3.x"
   from Bernd Wokoeck 2016

   Uses affordable Arduino Pro Mini + Ublox NEO-6M GPS-module

   Libraries needed
   - TinyGPS++ by Mikal Hart
   - GPSsensor by Bernd Wokoeck (Extended by Tero Salminen)
   - AltSoftSerial by Paul Stoffregen
   - Jeti Sensor EX Telemetry C++ Library by Bernd Wokoeck

   Tero Salminen RC-Thoughts.com (c) 2017 www.rc-thoughts.com

  -----------------------------------------------------------

                    Versatile features:

   User selectable EU or US mode. (km/h & meter / mph & feet)

   Simple mode values
   - Longitude
   - Latitude
   - Groundspeed
   - Altitude

   Extended mode added values
   - Variometer (VSI)
   - Distance to model from start point
   - Model heading (Flying direction)
   - Course to model from start point
   - Satellites count
   - HDOP (Horisontal dilution of precision)

  -----------------------------------------------------------
      Shared under MIT-license by Tero Salminen (c) 2017
  -----------------------------------------------------------
*/

#include <JetiExSerial.h>
#include <JetiExProtocol.h>
#include "GpsSensor.h"
#include <EEPROM.h>

// Libraries for different sensor-modules
#include <Adafruit_BMP085.h>
#include <Adafruit_BMP280.h>
#include <Adafruit_BME280.h>
Adafruit_BMP085 bmp085;
Adafruit_BMP280 bmp280;
Adafruit_BME280 bme280;

GpsSensor gps;
JetiExProtocol jetiEx;

// EU/US Units
enum unitType {EU, US};
unitType units = EU;

int extended = false;
int distance3D = true;
long homeTime = 0;
boolean fix = false;
boolean homeSet = false;
unsigned long distToHome = 0;
long courseToModel = 0;
float lat;
float lng;
float home_lat;
float home_lon;
float home_alt = 0;
int altirel;
int altiabs;

int uLoopCount = 0;
long uPressure = 0;
int uTemperature = 0;
long uVario = 0;
float lastVariofilter = 0;
long curAltitude = 0;
long lastAltitude = 0;
unsigned long lastTime = 0;
long startAltitude = 0;
int uHumidity = 0;

// Sensor type
enum senType {
  unknown,
  BMP085_BMP180,
  BMP280,
  BME280
};

struct {
  senType type = unknown;
  float filterX = 0.5;
  float filterY = 0.5;
  long deadzone = 0;
} pressureSensor;


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
  ID_HUMIDITY     = 14
};

JETISENSOR_CONST sensorsEU[] PROGMEM =
{
  // id             name          unit          data type           precision
  { ID_GPSLAT,      "Latitude",   " ",          JetiSensor::TYPE_GPS, 0 },
  { ID_GPSLON,      "Longitude",  " ",          JetiSensor::TYPE_GPS, 0 },
  { ID_GPSSPEED,    "Speed",      "km/h",       JetiSensor::TYPE_14b, 0 },
  { ID_ALTREL,      "Rel. Altit", "m",          JetiSensor::TYPE_14b, 0 },
  { ID_ALTABS,      "Altitude",   "m",          JetiSensor::TYPE_14b, 0 },
  { 0 }
};

JETISENSOR_CONST sensorsEUext[] PROGMEM =
{
  // id             name          unit          data type           precision
  { ID_GPSLAT,      "Latitude",   " ",          JetiSensor::TYPE_GPS, 0 },
  { ID_GPSLON,      "Longitude",  " ",          JetiSensor::TYPE_GPS, 0 },
  { ID_GPSSPEED,    "Speed",      "km/h",       JetiSensor::TYPE_14b, 0 },
  { ID_ALTREL,      "Rel. Altit", "m",          JetiSensor::TYPE_14b, 0 },
  { ID_ALTABS,      "Altitude",   "m",          JetiSensor::TYPE_14b, 0 },
  { ID_VARIO,       "Vario",      "m/s",        JetiSensor::TYPE_30b, 2 },
  { ID_DIST,        "Distance",   "m",          JetiSensor::TYPE_14b, 0 },
  { ID_HEADING,     "Heading",    "\xB0",       JetiSensor::TYPE_14b, 0 },
  { ID_COURSE,      "Course",     "\xB0",       JetiSensor::TYPE_14b, 0 },
  { ID_SATS,        "Satellites", " ",          JetiSensor::TYPE_14b, 0 },
  { ID_HDOP,        "HDOP",       " ",          JetiSensor::TYPE_14b, 2 },
  { ID_PRESSURE,    "Pressure",   "hPa",        JetiSensor::TYPE_30b, 2 },
  { ID_TEMPERATURE, "Temperature","\xB0\x43",   JetiSensor::TYPE_14b, 1 },
  { ID_HUMIDITY,    "Humidity",   "%rH",        JetiSensor::TYPE_14b, 1 },
  { 0 }
};


JETISENSOR_CONST sensorsUS[] PROGMEM =
{
  // id             name          unit          data type           precision
  { ID_GPSLAT,      "Latitude",   " ",          JetiSensor::TYPE_GPS, 0 },
  { ID_GPSLON,      "Longitude",  " ",          JetiSensor::TYPE_GPS, 0 },
  { ID_GPSSPEED,    "Speed",      "mph",        JetiSensor::TYPE_14b, 0 },
  { ID_ALTREL,      "Rel. Altit", "ft",         JetiSensor::TYPE_14b, 0 },
  { ID_ALTABS,      "Altitude",   "ft",         JetiSensor::TYPE_14b, 0 },
  { 0 }
};

JETISENSOR_CONST sensorsUSext[] PROGMEM =
{
  // id             name          unit          data type           precision
  { ID_GPSLAT,      "Latitude",   " ",          JetiSensor::TYPE_GPS, 0 },
  { ID_GPSLON,      "Longitude",  " ",          JetiSensor::TYPE_GPS, 0 },
  { ID_GPSSPEED,    "Speed",      "mph",        JetiSensor::TYPE_14b, 0 },
  { ID_ALTREL,      "Rel. Altit", "ft",         JetiSensor::TYPE_14b, 0 },
  { ID_ALTABS,      "Altitude",   "ft",         JetiSensor::TYPE_14b, 0 },
  { ID_VARIO,       "Vario",      "ft/s",       JetiSensor::TYPE_30b, 2 },
  { ID_DIST,        "Distance",   "ft.",        JetiSensor::TYPE_14b, 0 },
  { ID_HEADING,     "Heading",    "\xB0",       JetiSensor::TYPE_14b, 0 },
  { ID_COURSE,      "Course",     "\xB0",       JetiSensor::TYPE_14b, 0 },
  { ID_SATS,        "Satellites", " ",          JetiSensor::TYPE_14b, 0 },
  { ID_HDOP,        "HDOP",       " ",          JetiSensor::TYPE_14b, 2 },
  { ID_PRESSURE,    "Pressure",   "inHG",       JetiSensor::TYPE_30b, 2 },
  { ID_TEMPERATURE, "Temperature","\xB0\x46",   JetiSensor::TYPE_14b, 1 },
  { ID_HUMIDITY,    "Humidity",   "%rH",        JetiSensor::TYPE_14b, 1 },
  { 0 }
};

// Restart by user
void(* resetFunc) (void) = 0;

void setup()
{
  // identify sensor
  if (bmp085.begin()) {
    pressureSensor.type = BMP085_BMP180;
    pressureSensor.filterX = BMP085_FILTER_X;
    pressureSensor.filterY = BMP085_FILTER_Y;
    pressureSensor.deadzone = BMP085_DEADZONE;
  } else if (bmp280.begin()) {
    pressureSensor.type = BMP280;
    pressureSensor.filterX = BMx280_FILTER_X;
    pressureSensor.filterY = BMx280_FILTER_Y;
    pressureSensor.deadzone = BMx280_DEADZONE;
  } else if (bme280.begin()) {
    pressureSensor.type = BME280;
    pressureSensor.filterX = BMx280_FILTER_X;
    pressureSensor.filterY = BMx280_FILTER_Y;
    pressureSensor.deadzone = BMx280_DEADZONE;
  }

  units = EEPROM.read(0);
  extended = EEPROM.read(1);
  if (units == 255) {
    units = EU;
  }
  if (extended == 255) {
    extended = false;
  }
  gps.Init( GpsSensor::SERIAL3 );

  jetiEx.SetDeviceId( 0x76, 0x32 );

  if (units == EU && extended == false) {
    jetiEx.Start( "GPS", sensorsEU, JetiExProtocol::SERIAL2 );
  }
  if (units == EU && extended == true) {
    jetiEx.Start( "GPS", sensorsEUext, JetiExProtocol::SERIAL2 );
  }
  if (units == US && extended == false) {
    jetiEx.Start( "GPS", sensorsUS, JetiExProtocol::SERIAL2 );
  }
  if (units == US && extended == true) {
    jetiEx.Start( "GPS", sensorsUSext, JetiExProtocol::SERIAL2 );
  }
}

void loop()
{
  if(pressureSensor.type != unknown){
    // Read sensormodule values
    switch (pressureSensor.type) {
    case BMP085_BMP180 : {
        uPressure = bmp085.readPressure(); // In Pascal (100 Pa = 1 hPa = 1 mbar)
        curAltitude = bmp085.readAltitude(101325) * 100; // In Centimeter
        uTemperature = bmp085.readTemperature() * 10; // In Celsius ( x10 for one decimal)
        break;
      }
    case BMP280 : {
        uPressure = bmp280.readPressure(); // In Pascal (100 Pa = 1 hPa = 1 mbar)
        curAltitude = bmp280.readAltitude(1013.25) * 100; // In Centimeter
        uTemperature = bmp280.readTemperature() * 10; // In Celsius ( x10 for one decimal)
        break;
      }
    case BME280 : {
        uPressure = bme280.readPressure(); // In Pascal (100 Pa = 1 hPa = 1 mbar)
        curAltitude = bme280.readAltitude(1013.25) * 100; // In Centimeter
        uTemperature = bme280.readTemperature() * 10; // In Celsius ( x10 for one decimal)
        uHumidity = bme280.readHumidity() * 10; // In %rH
        break;
      }
    }
  
    // Set start-altitude in sensor-start
    if (uLoopCount == 19) {
      startAltitude = curAltitude;
      lastVariofilter = 0;
    }
    
    // Vario calculation
    if(float(millis() - lastTime > 150)){
      uVario = (curAltitude - lastAltitude) * (1000 / float(millis() - lastTime));
      lastTime = millis();

      lastAltitude = curAltitude;
    
      // Vario Filter
      float fX;
      float fY = uVario;
      fX = pressureSensor.filterX * lastVariofilter;
      fY = pressureSensor.filterY * fY;
      lastVariofilter = fX + fY;
      uVario = lastVariofilter;
      
      // Dead zone filter
      if (uVario > pressureSensor.deadzone) {
        uVario -= pressureSensor.deadzone;
      } else if (uVario < (pressureSensor.deadzone * -1)) {
        uVario -= (pressureSensor.deadzone * -1);
      } else {
        uVario = 0;
      }
    }
    
    // Altitude
    altirel = (curAltitude - startAltitude) / 100;
    
    if (uLoopCount < 20) {
      uLoopCount++;
      uVario = 0;
      altirel = 0;
    }

    // EU to US conversions
    // ft/s = m/s / 0.3048
    // inHG = hPa * 0,029529983071445
    // ft = m / 0.3048
    if (units == US) {
      altirel = altirel / 0.3048;
      uVario = uVario / 0.3048;
      uPressure = uPressure * 0.029529983071445;
      uTemperature = uTemperature * 1.8 + 320;
    }

    jetiEx.SetSensorValue( ID_VARIO, uVario );
    jetiEx.SetSensorValue( ID_ALTREL, altirel );
    jetiEx.SetSensorValue( ID_PRESSURE, uPressure );
    jetiEx.SetSensorValue( ID_TEMPERATURE, uTemperature );
    jetiEx.SetSensorValue( ID_HUMIDITY, uHumidity );
  }
  
  gps.DoGpsSensor();

  if (!fix) {
    if (gps.GetValid() && gps.GetAge() < 2000) {
      fix = true;
      if (!homeSet) {
        homeTime = (millis() + 5000);
      }
    } else {
      fix = false;
    }
  }

  if (fix) {
    lat = gps.GetLat();
    lng = gps.GetLon();
    if (units == EU) {
      jetiEx.SetSensorValueGPS( ID_GPSLAT, false, lat );
      jetiEx.SetSensorValueGPS( ID_GPSLON, true, lng );
      jetiEx.SetSensorValue( ID_ALTABS, gps.GetAltMe() );
      jetiEx.SetSensorValue( ID_GPSSPEED, gps.GetSpeedKm() );
    }else{
      jetiEx.SetSensorValueGPS( ID_GPSLAT, false, lat );
      jetiEx.SetSensorValueGPS( ID_GPSLON, true, lng );
      jetiEx.SetSensorValue( ID_ALTABS, gps.GetAltFt() );
      jetiEx.SetSensorValue( ID_GPSSPEED, gps.GetSpeedMi() );
    }
    if (extended) {
      jetiEx.SetSensorValue( ID_HEADING, gps.GetCourseDeg() );
      jetiEx.SetSensorValue( ID_SATS, gps.GetSats() );
      jetiEx.SetSensorValue( ID_HDOP, gps.GetHDOP() );
    }

    if (!homeSet && homeTime > 0 && (homeTime < millis())) {
      homeSet = true;
      home_lat = lat;
      home_lon = lng;
      if (units == EU) {
        home_alt = gps.GetAltMe();
      }else{
        home_alt = gps.GetAltFt();
      }
    }

    if (homeSet) {
      if (extended) {
        // Distance to Model
        distToHome = TinyGPSPlus::distanceBetween(
                      gps.GetLat(),
                      gps.GetLon(),
                      home_lat,
                      home_lon);
        if(distance3D){
          distToHome = sqrt(pow(altirel,2) + pow(distToHome,2));
        }
        if (units == US) {
          distToHome = (distToHome * 3.2808399);
        }

        // Course from home to model
        courseToModel =
          TinyGPSPlus::courseTo(
            home_lat,
            home_lon,
            gps.GetLat(),
            gps.GetLon());
      }

      // Calculate altitude from zero
      if (units == EU) {
        altiabs = gps.GetAltMe();
        calcRelAltitude();
        jetiEx.SetSensorValue( ID_ALTABS, altiabs );
        if(pressureSensor.type == unknown){
          jetiEx.SetSensorValue( ID_ALTREL, altirel );
        }
      }else{
        altiabs = gps.GetAltFt();
        calcRelAltitude();
        jetiEx.SetSensorValue( ID_ALTABS, altiabs );
        if(pressureSensor.type == unknown){
          jetiEx.SetSensorValue( ID_ALTREL, altirel );
        }
      }
    } else {
      distToHome = 0;
      courseToModel = 0;
    }
    if (extended) {
      jetiEx.SetSensorValue( ID_DIST, distToHome );
      jetiEx.SetSensorValue( ID_COURSE, courseToModel );
    }
  } else { // If Fix end
    lat = 0;
    lng = 0;
    if(pressureSensor.type == unknown){
      jetiEx.SetSensorValue( ID_ALTREL, 0 );
    }
    jetiEx.SetSensorValueGPS( ID_GPSLAT, false, lat );
    jetiEx.SetSensorValueGPS( ID_GPSLON, true, lng );
    jetiEx.SetSensorValue( ID_ALTABS, 0 );
    jetiEx.SetSensorValue( ID_GPSSPEED, 0 );
    if (extended) {
      jetiEx.SetSensorValue( ID_HEADING, 0 );
      jetiEx.SetSensorValue( ID_SATS, 0 );
      jetiEx.SetSensorValue( ID_HDOP, 0 );
      jetiEx.SetSensorValue( ID_DIST, 0 );
      jetiEx.SetSensorValue( ID_COURSE, 0 );
    }
  }

  HandleMenu();
  jetiEx.DoJetiSend();
}

void calcRelAltitude(){
  if (home_alt >= 0) {
    altirel = (altiabs - home_alt);
    if (altirel < 0) {
      altirel = 0;
    }
  } else {
    altirel = (altiabs + home_alt);
    if (altirel < 0) {
      altirel = 0;
    }
  }
}

void HandleMenu()
{
  static int  _nMenu = 0;
  static bool _bSetDisplay = true;
  uint8_t c = jetiEx.GetJetiboxKey();

  // 224 0xe0 : // RIGHT
  // 112 0x70 : // LEFT
  // 208 0xd0 : // UP
  // 176 0xb0 : // DOWN
  // 144 0x90 : // UP+DOWN
  //  96 0x60 : // LEFT+RIGHT

  // Right
  if ( c == 0xe0 && _nMenu < 7 )
  {
    _nMenu++;
    _bSetDisplay = true;
  }

  // Left
  if ( c == 0x70 &&  _nMenu > 0 )
  {
    _nMenu--;
    _bSetDisplay = true;
  }

  // DN
  if ( c == 0xb0 )
  {
    if ( _nMenu == 1 ) {
      units = EU;
      EEPROM.write(0, units);
      _nMenu = 8;
      _bSetDisplay = true;
      resetFunc();
    }
    if ( _nMenu == 2 ) {
      units = US;
      EEPROM.write(0, units);
      _nMenu = 8;
      _bSetDisplay = true;
      resetFunc();
    }
    if ( _nMenu == 3 ) {
      extended = false;
      EEPROM.write(1, extended);
      _nMenu = 8;
      _bSetDisplay = true;
      resetFunc();
    }
    if ( _nMenu == 4 ) {
      extended = true;
      EEPROM.write(1, extended);
      _nMenu = 8;
      _bSetDisplay = true;
      resetFunc();
    }
    if ( _nMenu == 5 ) {
      distance3D = false;
      EEPROM.write(2, distance3D);
      _nMenu = 8;
      _bSetDisplay = true;
      resetFunc();
    }
    if ( _nMenu == 6 ) {
      distance3D = true;
      EEPROM.write(2, distance3D);
      _nMenu = 8;
      _bSetDisplay = true;
      resetFunc();
    }
    if ( _nMenu == 7 ) {
      units = EU;
      extended = false;
      distance3D = true;
      EEPROM.write(0, units);
      EEPROM.write(1, extended);
      EEPROM.write(2, distance3D);
      _nMenu = 8;
      _bSetDisplay = true;
      resetFunc();
    }
  }

  if ( !_bSetDisplay )
    return;

  switch ( _nMenu )
  {
    case 0:
      jetiEx.SetJetiboxText( JetiExProtocol::LINE1, " RCT Jeti Tools" );
      jetiEx.SetJetiboxText( JetiExProtocol::LINE2, "GPS Sensor V1.6" );
      _bSetDisplay = false;
      break;
    case 1:
      jetiEx.SetJetiboxText( JetiExProtocol::LINE1, "  Meter & km/h" );
      jetiEx.SetJetiboxText( JetiExProtocol::LINE2, " Restart: DOWN" );
      _bSetDisplay = false;
      break;
    case 2:
      jetiEx.SetJetiboxText( JetiExProtocol::LINE1, "  Feet & mph" );
      jetiEx.SetJetiboxText( JetiExProtocol::LINE2, " Restart: DOWN" );
      _bSetDisplay = false;
      break;
    case 3:
      jetiEx.SetJetiboxText( JetiExProtocol::LINE1, "  Basic mode" );
      jetiEx.SetJetiboxText( JetiExProtocol::LINE2, " Restart: DOWN" );
      _bSetDisplay = false;
      break;
    case 4:
      jetiEx.SetJetiboxText( JetiExProtocol::LINE1, " Extended mode" );
      jetiEx.SetJetiboxText( JetiExProtocol::LINE2, " Restart: DOWN" );
      _bSetDisplay = false;
      break;
    case 5:
      jetiEx.SetJetiboxText( JetiExProtocol::LINE1, "Horiz. distance" );
      jetiEx.SetJetiboxText( JetiExProtocol::LINE2, " Restart: DOWN" );
      _bSetDisplay = false;
      break;
    case 6:
      jetiEx.SetJetiboxText( JetiExProtocol::LINE1, "  3D distance" );
      jetiEx.SetJetiboxText( JetiExProtocol::LINE2, " Restart: DOWN" );
      _bSetDisplay = false;
      break;
    case 7:
      jetiEx.SetJetiboxText( JetiExProtocol::LINE1, " Reset defaults" );
      jetiEx.SetJetiboxText( JetiExProtocol::LINE2, " Restart: DOWN" );
      _bSetDisplay = false;
      break;
  }
}
