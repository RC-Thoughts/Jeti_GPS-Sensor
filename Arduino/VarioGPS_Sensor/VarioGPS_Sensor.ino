/*
  -----------------------------------------------------------
            Jeti VarioGPS Sensor
  -----------------------------------------------------------
*/
#define VARIOGPS_VERSION "Version V1.7.2"
/*

   Based on the "Jeti GPS Sensor" from Tero Salminen 2017

   Hardware:
   - Arduino Pro Mini
   - Ublox GPS-module
   - Pressure module with BMP085, BMP180, BMP280, BME280 

   Libraries needed
   - TinyGPS++ by Mikal Hart
   - GPSsensor by Bernd Wokoeck (Extended by Tero Salminen)
   - AltSoftSerial by Paul Stoffregen
   - Jeti Sensor EX Telemetry C++ Library by Bernd Wokoeck
   - Adafruit BMP085, BMP280, BME280 

  -----------------------------------------------------------

                    Versatile features:

   User selectable EU or US mode. (km/h & meter / mph & feet)

   Simple mode values
   - Longitude
   - Latitude
   - Groundspeed
   - Altitude
   - Vario

   Extended mode added values
   - Distance to model from start point
   - Model heading (Flying direction)
   - Course to model from start point
   - Satellites count
   - HDOP (Horisontal dilution of precision)
   - Pressure
   - Temperature
   - Humidity

   Two external voltages can be measured   
   
*/


#include <JetiExSerial.h>
#include <JetiExProtocol.h>
#include "GpsSensor.h"
#include <EEPROM.h>
#include "BMx_Sensor.h"

BMx_Sensor boschPressureSensor;

GpsSensor gps;
JetiExProtocol jetiEx;

long homeTime = 0;
bool fix = false;
bool homeSet = false;
unsigned long distToHome = 0;
long courseToModel = 0;
float lat;
float lng;
float home_lat;
float home_lon;

int uLoopCount = 0;
long uPressure = 0;
int uTemperature = 0;
long uVario = 0;
float lastVariofilter = 0;
long curAltitude = 0;
long lastAltitude = 0;
unsigned long lastTime = 0;
long startAltitude = 0;
long uRelAltitude = 0;
long uAbsAltitude = 0;
int uHumidity = 0;
bool volt1Enable;
bool volt2Enable;

// EU/US Units
enum unitType {EU, US};
unitType units;

// GPS mode
enum gpsmodes{
  disabled,
  basic,
  extended
};

// GPS
struct {
  gpsmodes mode;
  bool distance3D;
} gpsSettings;

// Pressure Sensor
struct {
  int type = unknown;
  float filterX = 0.5;
  float filterY = 0.5;
  long deadzone = 0;
} pressureSensor;

#include "defaults.h"

// Restart by user
void(* resetFunc) (void) = 0;

#include "HandleMenu.h"

void setup()
{
  units = DEFAULT_UNIT;
  gpsSettings.mode = DEFAULT_GPS_MODE;
  gpsSettings.distance3D = DEFAULT_GPS_3D_DISTANCE;
  volt1Enable = DEFAULT_VOLT1_ENABLE;
  volt2Enable = DEFAULT_VOLT2_ENABLE;
  
  // identify sensor
  pressureSensor.type = boschPressureSensor.begin(0x76);
  if(pressureSensor.type == unknown){
    pressureSensor.type = boschPressureSensor.begin(0x77);
  }
  switch (pressureSensor.type){
    case BMP085_BMP180:
      pressureSensor.filterX = BMP085_FILTER_X;
      pressureSensor.filterY = BMP085_FILTER_Y;
      pressureSensor.deadzone = BMP085_DEADZONE;
      break;
    case BMP280:
      pressureSensor.filterX = BMx280_FILTER_X;
      pressureSensor.filterY = BMx280_FILTER_Y;
      pressureSensor.deadzone = BMx280_DEADZONE;
      break;
    case BME280:
      pressureSensor.filterX = BMx280_FILTER_X;
      pressureSensor.filterY = BMx280_FILTER_Y;
      pressureSensor.deadzone = BMx280_DEADZONE;
      break;
  }
 

  // read settings from eeprom
  if (EEPROM.read(0) != 0xFF) {
    units = EEPROM.read(0);
  } 
  if (EEPROM.read(1) != 0xFF) {
    gpsSettings.mode = EEPROM.read(1);
  }
  if (EEPROM.read(2) != 0xFF) {
    gpsSettings.distance3D = EEPROM.read(2);
  }
  if (EEPROM.read(3) != 0xFF) {
    volt1Enable = EEPROM.read(3);
  }
  if (EEPROM.read(4) != 0xFF) {
    volt2Enable = EEPROM.read(4);
  }

  // init GPS
  gps.Init( GpsSensor::SERIAL3 );

  // Setup sensors
  if(pressureSensor.type == unknown){
    jetiEx.SetSensorActive( ID_VARIO, false, sensorsEU );
    jetiEx.SetSensorActive( ID_VARIO, false, sensorsUS );
  }
  
  if(gpsSettings.mode == basic || pressureSensor.type != BME280){
    jetiEx.SetSensorActive( ID_HUMIDITY, false, sensorsEU );
    jetiEx.SetSensorActive( ID_HUMIDITY, false, sensorsUS );
  }
  if(gpsSettings.mode == basic || pressureSensor.type == unknown){
    jetiEx.SetSensorActive( ID_PRESSURE, false, sensorsEU );
    jetiEx.SetSensorActive( ID_TEMPERATURE, false, sensorsEU );

    jetiEx.SetSensorActive( ID_PRESSURE, false, sensorsUS );
    jetiEx.SetSensorActive( ID_TEMPERATURE, false, sensorsUS );
  }

  if(gpsSettings.mode == disabled){
    jetiEx.SetSensorActive( ID_GPSLAT, false, sensorsEU );
    jetiEx.SetSensorActive( ID_GPSLON, false, sensorsEU );
    jetiEx.SetSensorActive( ID_GPSSPEED, false, sensorsEU );

    jetiEx.SetSensorActive( ID_GPSLAT, false, sensorsUS );
    jetiEx.SetSensorActive( ID_GPSLON, false, sensorsUS );
    jetiEx.SetSensorActive( ID_GPSSPEED, false, sensorsUS );
  }

  if(gpsSettings.mode == disabled && pressureSensor.type == unknown){
    jetiEx.SetSensorActive( ID_ALTREL, false, sensorsEU );
    jetiEx.SetSensorActive( ID_ALTABS, false, sensorsEU );

    jetiEx.SetSensorActive( ID_ALTREL, false, sensorsUS );
    jetiEx.SetSensorActive( ID_ALTABS, false, sensorsUS );
  }
  
  if(gpsSettings.mode != extended){
    jetiEx.SetSensorActive( ID_DIST, false, sensorsEU );
    jetiEx.SetSensorActive( ID_HEADING, false, sensorsEU );
    jetiEx.SetSensorActive( ID_COURSE, false, sensorsEU );
    jetiEx.SetSensorActive( ID_SATS, false, sensorsEU );
    jetiEx.SetSensorActive( ID_HDOP, false, sensorsEU );

    jetiEx.SetSensorActive( ID_DIST, false, sensorsUS );
    jetiEx.SetSensorActive( ID_HEADING, false, sensorsUS );
    jetiEx.SetSensorActive( ID_COURSE, false, sensorsUS );
    jetiEx.SetSensorActive( ID_SATS, false, sensorsUS );
    jetiEx.SetSensorActive( ID_HDOP, false, sensorsUS );
  }

  if(volt1Enable == false){
    jetiEx.SetSensorActive( ID_V1, false, sensorsEU );
    jetiEx.SetSensorActive( ID_V1, false, sensorsUS );
  }

  if(volt2Enable == false){
    jetiEx.SetSensorActive( ID_V2, false, sensorsEU );
    jetiEx.SetSensorActive( ID_V2, false, sensorsUS );
  }

  jetiEx.SetDeviceId( 0x76, 0x32 );

  if (units == EU) {
    jetiEx.Start( "GPS", sensorsEU, JetiExProtocol::SERIAL2 );
  }else{
    jetiEx.Start( "GPS", sensorsUS, JetiExProtocol::SERIAL2 );
  }
}

void loop()
{
  if(pressureSensor.type != unknown){
    // Read sensormodule values
    uPressure = boschPressureSensor.readPressure(); // In Pascal (100 Pa = 1 hPa = 1 mbar)
    curAltitude = boschPressureSensor.readAltitude() * 100; // In Centimeter
    uTemperature = boschPressureSensor.readTemperature() * 10; // In Celsius ( x10 for one decimal)
    if(pressureSensor.type == BME280){
      uHumidity = boschPressureSensor.readHumidity() * 10; // In %rH
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
    uRelAltitude = (curAltitude - startAltitude) / 10;
    uAbsAltitude = curAltitude / 100;
    
    if (uLoopCount < 20) {
      uLoopCount++;
      uVario = 0;
      uRelAltitude = 0;
      uAbsAltitude = 0;
    }
  }

  if(gpsSettings.mode != disabled){
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
        jetiEx.SetSensorValue( ID_GPSSPEED, gps.GetSpeedKm() );
      }else{
        jetiEx.SetSensorValue( ID_GPSSPEED, gps.GetSpeedMi() );
      }
      jetiEx.SetSensorValue( ID_HEADING, gps.GetCourseDeg() );
  
      if (!homeSet && homeTime > 0 && (homeTime < millis())) {
        homeSet = true;
        home_lat = lat;
        home_lon = lng;
        if(pressureSensor.type == unknown){
          startAltitude = gps.GetAltMe();
        }
      }
  
      // Altitude
      uAbsAltitude = gps.GetAltMe();
      if(pressureSensor.type == unknown){
        uRelAltitude = (uAbsAltitude - startAltitude)*10;
      }
  
      if (homeSet) {
        // Distance to Model
        distToHome = TinyGPSPlus::distanceBetween(
                      gps.GetLat(),
                      gps.GetLon(),
                      home_lat,
                      home_lon);
        if(gpsSettings.distance3D){
          distToHome = sqrt(pow(uRelAltitude/10,2) + pow(distToHome,2));
        }
  
        // Course from home to model
        courseToModel =
          TinyGPSPlus::courseTo(
            home_lat,
            home_lon,
            gps.GetLat(),
            gps.GetLon());
      } else {
        distToHome = 0;
        courseToModel = 0;
      }
      
    } else { // If Fix end
      lat = 0;
      lng = 0;
      if(pressureSensor.type == unknown){
        uRelAltitude = 0;
      }
      uAbsAltitude = 0;
      distToHome = 0;
      courseToModel = 0;
      jetiEx.SetSensorValue( ID_GPSSPEED, 0 );
      jetiEx.SetSensorValue( ID_HEADING, 0 );
    }

    jetiEx.SetSensorValueGPS( ID_GPSLAT, false, lat );
    jetiEx.SetSensorValueGPS( ID_GPSLON, true, lng );
    jetiEx.SetSensorValue( ID_COURSE, courseToModel );
    jetiEx.SetSensorValue( ID_SATS, gps.GetSats() );
    jetiEx.SetSensorValue( ID_HDOP, gps.GetHDOP() );
  }
  
  // EU to US conversions
  // ft/s = m/s / 0.3048
  // inHG = hPa * 0,029529983071445
  // ft = m / 0.3048
  if (units == US) {
    uRelAltitude /= 0.3048;
    uAbsAltitude /= 0.3048;
    uVario /= 0.3048;
    distToHome *= 3.2808399;
    uPressure *= 0.029529983071445;
    uTemperature = uTemperature * 1.8 + 320;
  } 
    
  
  jetiEx.SetSensorValue( ID_ALTREL, uRelAltitude );
  jetiEx.SetSensorValue( ID_ALTABS, uAbsAltitude );
  jetiEx.SetSensorValue( ID_VARIO, uVario );
  jetiEx.SetSensorValue( ID_DIST, distToHome );
  jetiEx.SetSensorValue( ID_PRESSURE, uPressure );
  jetiEx.SetSensorValue( ID_TEMPERATURE, uTemperature );
  jetiEx.SetSensorValue( ID_HUMIDITY, uHumidity );

  if(volt1Enable){
    jetiEx.SetSensorValue( ID_V1, float(analogRead(PIN_V1)*V_REF/1024)*(float(VOLTAGE1_R1+VOLTAGE1_R2)/VOLTAGE1_R2)*100);
  }

  if(volt2Enable){
    jetiEx.SetSensorValue( ID_V2, float(analogRead(PIN_V2)*V_REF/1024)*(float(VOLTAGE2_R1+VOLTAGE2_R2)/VOLTAGE2_R2)*100);
  }

  HandleMenu();
  jetiEx.DoJetiSend();
}
