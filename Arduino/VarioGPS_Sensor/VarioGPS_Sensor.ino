/*
  -----------------------------------------------------------
            Jeti VarioGPS Sensor
  -----------------------------------------------------------
*/
#define VARIOGPS_VERSION "Version V1.8"
/*

  Hardware:
  - Arduino Pro Mini 8Mhz 3.3V
  - Ublox GPS-Modul mit 9600baud
  - Barometer Modul mit BMP280, BME280, MS5611, LPS
  
  
  Ohne GPS mit Barometer werden die Werte angezeigt:
  - Rel. und Abs. Höhe
  - Vario
  - Luftdruck
  - Temperatur
  - Luftfeuchte (nur mit BME280)
  
  Im GPS Basic Mode werden die Werte angezeigt:
  - Position
  - Geschwindigkeit 
  - Rel. und Abs. Höhe
  - Vario
  
  Im GPS Extended Mode werden zusätzlich die Werte angezeigt:
  - Distanz vom Modell zum Startpunkt (2D und 3D)
  - Flugrichtung (Heading)
  - Kurs vom Modell zum Startpunkt
  - Anzahl Satelliten
  - HDOP (Horizontaler Faktor der Positionsgenauigkeit)
  - Luftdruck
  - Temperatur
  - Luftfeuchtigkeit 
  
  Zusätzlich können bis zu 4 Spannungen gemessen werden
  
  Folgende Einstellungen können per Jetibox vorgenommen werden:
  
  - GPS: deaktiviert, Basic oder Extended
  - GPS Distanz: 2D oder 3D
  - Filterparameter X, Y und Deadzone
  - Spannungsmessungen 1-4 de-/aktiviert
   
*/

#include <JetiExSerial.h>
#include <JetiExProtocol.h>
#include <EEPROM.h>
#include "GpsSensor.h"
#include "BMx_Sensor.h"
#include <MS5611.h>
#include <LPS.h>

BMx_Sensor boschPressureSensor;
MS5611 ms5611;
LPS lps;

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

// EU/US Units
enum unitType {EU, US};
unitType units;

// GPS
enum {
  GPS_disabled,
  GPS_basic,
  GPS_extended
};

struct {
  uint8_t mode;
  bool distance3D;
} gpsSettings;


// Pressure Sensors
enum {
  unknown,
  BMP280,
  BME280,
  MS5611,
  LPS
};

struct {
  uint8_t type = unknown;
  float filterX;
  float filterY;
  long deadzone;
} pressureSensor;


// Analog input
enum {
  analog_disabled,
  analog_enabled
};

#include "defaults.h"

uint8_t analogInputMode[MAX_ANALOG_INPUTS];

// Restart by user
void(* resetFunc) (void) = 0;

#include "HandleMenu.h"

void setup()
{
  // set defaults
  units = DEFAULT_UNIT;
  gpsSettings.mode = DEFAULT_GPS_MODE;
  gpsSettings.distance3D = DEFAULT_GPS_3D_DISTANCE;
  analogInputMode[0] = DEFAULT_MODE_ANALOG_1;
  analogInputMode[1] = DEFAULT_MODE_ANALOG_2;
  analogInputMode[2] = DEFAULT_MODE_ANALOG_3;
  analogInputMode[3] = DEFAULT_MODE_ANALOG_4;
  
  // identify sensor
  pressureSensor.type = boschPressureSensor.begin(0x76);
  if(pressureSensor.type == unknown){
    pressureSensor.type = boschPressureSensor.begin(0x77);
  }
  if(pressureSensor.type == unknown){
    if (lps.init()) {
      Wire.begin();
      lps.enableDefault();
      pressureSensor.type = LPS;
    } else {
      Wire.beginTransmission(MS5611_ADDRESS); // if no Bosch sensor, check if return an ACK on MS5611 address
      if (Wire.endTransmission() == 0) {
        ms5611.begin(MS5611_ULTRA_HIGH_RES);
        pressureSensor.type = MS5611;
      }
    }
  }
  
  switch (pressureSensor.type){
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
    case MS5611 :
      pressureSensor.filterX = MS5611_FILTER_X;
      pressureSensor.filterY = MS5611_FILTER_Y;
      pressureSensor.deadzone = MS5611_DEADZONE;
      break;
    case LPS :
      pressureSensor.filterX = LPS_FILTER_X;
      pressureSensor.filterY = LPS_FILTER_Y;
      pressureSensor.deadzone = LPS_DEADZONE;
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
  for(uint8_t i=0; i < MAX_ANALOG_INPUTS; i++){
    if (EEPROM.read(3+i) != 0xFF) {
      analogInputMode[i] = EEPROM.read(3+i);
    }
  }
  if (EEPROM.read(10) != 0xFF) {
    pressureSensor.filterX = (float)EEPROM.read(10) / 100;
  }
  if (EEPROM.read(11) != 0xFF) {
    pressureSensor.filterY = (float)EEPROM.read(11) / 100;
  }
  if (EEPROM.read(12) != 0xFF) {
    pressureSensor.deadzone = EEPROM.read(12);
  }

  // init GPS
  gps.Init( GpsSensor::SERIAL3 );

  // Setup sensors
  if(pressureSensor.type == unknown){
    jetiEx.SetSensorActive( ID_VARIO, false, sensorsEU );
    jetiEx.SetSensorActive( ID_VARIO, false, sensorsUS );
  }
  
  if(gpsSettings.mode == GPS_basic || pressureSensor.type != BME280){
    jetiEx.SetSensorActive( ID_HUMIDITY, false, sensorsEU );
    jetiEx.SetSensorActive( ID_HUMIDITY, false, sensorsUS );
  }
  if(gpsSettings.mode == GPS_basic || pressureSensor.type == unknown){
    jetiEx.SetSensorActive( ID_PRESSURE, false, sensorsEU );
    jetiEx.SetSensorActive( ID_TEMPERATURE, false, sensorsEU );

    jetiEx.SetSensorActive( ID_PRESSURE, false, sensorsUS );
    jetiEx.SetSensorActive( ID_TEMPERATURE, false, sensorsUS );
  }

  if(gpsSettings.mode == GPS_disabled){
    jetiEx.SetSensorActive( ID_GPSLAT, false, sensorsEU );
    jetiEx.SetSensorActive( ID_GPSLON, false, sensorsEU );
    jetiEx.SetSensorActive( ID_GPSSPEED, false, sensorsEU );

    jetiEx.SetSensorActive( ID_GPSLAT, false, sensorsUS );
    jetiEx.SetSensorActive( ID_GPSLON, false, sensorsUS );
    jetiEx.SetSensorActive( ID_GPSSPEED, false, sensorsUS );
  }

  if(gpsSettings.mode == GPS_disabled && pressureSensor.type == unknown){
    jetiEx.SetSensorActive( ID_ALTREL, false, sensorsEU );
    jetiEx.SetSensorActive( ID_ALTABS, false, sensorsEU );

    jetiEx.SetSensorActive( ID_ALTREL, false, sensorsUS );
    jetiEx.SetSensorActive( ID_ALTABS, false, sensorsUS );
  }
  
  if(gpsSettings.mode != GPS_extended){
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

  for(uint8_t i=0; i < MAX_ANALOG_INPUTS; i++){
    if (analogInputMode[i] == analog_disabled) {
      jetiEx.SetSensorActive( ID_V1+i, false, sensorsEU );
      jetiEx.SetSensorActive( ID_V1+i, false, sensorsUS );
    }
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
  if(pressureSensor.type != unknown && (millis() - lastTime) > 150){
    // Read sensormodule values
    if(pressureSensor.type == MS5611){
      uPressure = ms5611.readPressure(true); // In Pascal (100 Pa = 1 hPa = 1 mbar)
      curAltitude = ms5611.getAltitude(uPressure, 101325) * 100; // In Centimeter
      uTemperature = ms5611.readTemperature(true) * 10; // In Celsius ( x10 for one decimal)
    }else if(pressureSensor.type == LPS){
      uPressure = lps.readPressureMillibars(); // In Pascal (100 Pa = 1 hPa = 1 mbar)
      curAltitude = lps.pressureToAltitudeMeters(uPressure) * 100; // In Centimeter
      uTemperature = lps.readTemperatureC() * 10; // In Celsius ( x10 for one decimal)
    }else{
      uPressure = boschPressureSensor.readPressure(); // In Pascal (100 Pa = 1 hPa = 1 mbar)
      curAltitude = boschPressureSensor.readAltitude() * 100; // In Centimeter
      uTemperature = boschPressureSensor.readTemperature() * 10; // In Celsius ( x10 for one decimal)
      if(pressureSensor.type == BME280){
        uHumidity = boschPressureSensor.readHumidity() * 10; // In %rH
      }
    }
     
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
    
    if (millis() < 5000) {
      // Set start-altitude in sensor-start
      startAltitude = curAltitude;
      lastVariofilter = 0;
      uVario = 0;
      uRelAltitude = 0;
      uAbsAltitude = 0;
    }else{
      // Altitude
      uRelAltitude = (curAltitude - startAltitude) / 10;
      uAbsAltitude = curAltitude / 100;
    }
  }

  if(gpsSettings.mode != GPS_disabled){
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

  for(uint8_t i=0; i < MAX_ANALOG_INPUTS; i++){
    if (analogInputMode[i] == analog_enabled) {
      jetiEx.SetSensorValue( ID_V1+i, float(analogRead(analogInputPin[i])*V_REF/1024)*(float(analogInputR1[i]+analogInputR2[i])/analogInputR2[i])*100);
    }
  }

  HandleMenu();
  jetiEx.DoJetiSend();
}
