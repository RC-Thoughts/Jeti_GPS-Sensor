/*
  -----------------------------------------------------------
            Jeti VarioGPS Sensor
  -----------------------------------------------------------
*/
#define VARIOGPS_VERSION "Version V2.0.2b"
/*

  Unterstützte Hardware:
  - Arduino Pro Mini 
  - GPS-Modul mit UART@9600baud
  - Luftdrucksensoren: BMP280, BME280, MS5611, LPS 
  - Stromsensoren @3.3V Betriebsspannung: ACS758_50B, ACS758_100B, ACS758_150B, ACS758_200B, ACS758_50U, ACS758_100U, ACS758_150U, ACS758_200U
  - zusätzliche Stromsensoren @5V Betriebsspannung: ACS712_05, ACS712_20, ACS712_30
  
  
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
  - zurückgelegte Strecke
  - Flugrichtung (Heading)
  - Kurs vom Modell zum Startpunkt
  - Anzahl Satelliten
  - HDOP (Horizontaler Faktor der Positionsgenauigkeit)
  - Luftdruck
  - Temperatur
  - Luftfeuchtigkeit 

  An den Analogeingängen A0-A1 sind folgende Messungen möglich:
  - Spannungsmessung
  - Strommessung mit verbrauchter Energie
  
  Folgende Einstellungen können per Jetibox vorgenommen werden:
  - GPS: deaktiviert, Basic oder Extended
  - GPS Distanz: 2D oder 3D
  - Vario Filterparameter X, Y und Deadzone
  - Analogeingänge 1-4 konfigurierbar (deaktiviert, Spannungs- oder Strommessung)
   
*/

#include <JetiExSerial.h>
#include <JetiExProtocol.h>
#include <EEPROM.h>
#include <Wire.h>
#include "defaults.h"

JetiExProtocol jetiEx;

#ifdef SUPPORT_GPS 
  #include <TinyGPS++.h>
  #include <AltSoftSerial.h>
  TinyGPSPlus gps;
  AltSoftSerial gpsSerial;
#endif

#ifdef SUPPORT_BMx280 
  #include "BMx_Sensor.h"
  BMx_Sensor boschPressureSensor;
#endif

#ifdef SUPPORT_MS5611_LPS
  #include <MS5611.h>
  #include <LPS.h>
  MS5611 ms5611;
  LPS lps;
#endif

struct {
  uint8_t mode;
  bool distance3D;
} gpsSettings;

struct {
  uint8_t type = unknown;
  float filterX;
  float filterY;
  long deadzone;
} pressureSensor;


const float factorVoltageDivider[MAX_ANALOG_INPUTS] = { float(analogInputR1[0]+analogInputR2[0])/analogInputR2[0],
                                                        float(analogInputR1[1]+analogInputR2[1])/analogInputR2[1],
                                                        float(analogInputR1[2]+analogInputR2[2])/analogInputR2[2],
                                                        float(analogInputR1[3]+analogInputR2[3])/analogInputR2[3]  };

const float timefactorPowerConsumption = (1000.0/MEASURING_INTERVAL*60*60)/1000;
uint8_t analogInputMode[MAX_ANALOG_INPUTS];
float powerConsumption[MAX_ANALOG_INPUTS] = {0,0,0,0};

// Restart by user
void(* resetFunc) (void) = 0;

#include "HandleMenu.h"

void setup()
{
  // set defaults
  gpsSettings.mode = DEFAULT_GPS_MODE;
  gpsSettings.distance3D = DEFAULT_GPS_3D_DISTANCE;
  analogInputMode[0] = DEFAULT_MODE_ANALOG_1;
  analogInputMode[1] = DEFAULT_MODE_ANALOG_2;
  analogInputMode[2] = DEFAULT_MODE_ANALOG_3;
  analogInputMode[3] = DEFAULT_MODE_ANALOG_4;
  
  // identify sensor
  #ifdef SUPPORT_BMx280 
    pressureSensor.type = boschPressureSensor.begin(0x76);
    if(pressureSensor.type == unknown){
      pressureSensor.type = boschPressureSensor.begin(0x77);
    }
  #endif 
  #ifdef SUPPORT_MS5611_LPS  
    if(pressureSensor.type == unknown){
      if (lps.init()) {
        Wire.begin();
        lps.enableDefault();
        pressureSensor.type = LPS_;
      } else {
        Wire.beginTransmission(MS5611_ADDRESS); // if no Bosch sensor, check if return an ACK on MS5611 address
        if (Wire.endTransmission() == 0) {
          ms5611.begin(MS5611_ULTRA_HIGH_RES);
          pressureSensor.type = MS5611_;
        }
      }
    }
  #endif
  
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
    case MS5611_ :
      pressureSensor.filterX = MS5611_FILTER_X;
      pressureSensor.filterY = MS5611_FILTER_Y;
      pressureSensor.deadzone = MS5611_DEADZONE;
      break;
    case LPS_ :
      pressureSensor.filterX = LPS_FILTER_X;
      pressureSensor.filterY = LPS_FILTER_Y;
      pressureSensor.deadzone = LPS_DEADZONE;
      break;
  }
 
  // read settings from eeprom 
  #ifdef SUPPORT_GPS
    if (EEPROM.read(1) != 0xFF) {
      gpsSettings.mode = EEPROM.read(1);
    }
  #endif
  
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

  #ifdef SUPPORT_GPS
    // init GPS
    gpsSerial.begin(GPSBaud);
  #endif
  

  // Setup sensors
  if(pressureSensor.type == unknown){
    jetiEx.SetSensorActive( ID_VARIO, false, sensors );
  }
  
  if(gpsSettings.mode == GPS_basic || pressureSensor.type != BME280){
    jetiEx.SetSensorActive( ID_HUMIDITY, false, sensors );
  }
  if(gpsSettings.mode == GPS_basic || pressureSensor.type == unknown){
    jetiEx.SetSensorActive( ID_PRESSURE, false, sensors );
    jetiEx.SetSensorActive( ID_TEMPERATURE, false, sensors );
  }

  if(gpsSettings.mode == GPS_disabled){
    jetiEx.SetSensorActive( ID_GPSLAT, false, sensors );
    jetiEx.SetSensorActive( ID_GPSLON, false, sensors );
    jetiEx.SetSensorActive( ID_GPSSPEED, false, sensors );
  }

  if(gpsSettings.mode == GPS_disabled && pressureSensor.type == unknown){
    jetiEx.SetSensorActive( ID_ALTREL, false, sensors );
    jetiEx.SetSensorActive( ID_ALTABS, false, sensors );
  }
  
  if(gpsSettings.mode != GPS_extended){
    jetiEx.SetSensorActive( ID_DIST, false, sensors );
    jetiEx.SetSensorActive( ID_TRIP, false, sensors );
    jetiEx.SetSensorActive( ID_HEADING, false, sensors );
    jetiEx.SetSensorActive( ID_COURSE, false, sensors );
    jetiEx.SetSensorActive( ID_SATS, false, sensors );
    jetiEx.SetSensorActive( ID_HDOP, false, sensors );
  }

  for(uint8_t i=0; i < MAX_ANALOG_INPUTS; i++){
    if (analogInputMode[i] > voltage) {
      // if voltage
      jetiEx.SetSensorActive( ID_V1+i, false, sensors );
    }else if(analogInputMode[i] == voltage){
      // if current
      jetiEx.SetSensorActive( ID_A1+i, false, sensors );
      jetiEx.SetSensorActive( ID_C1+i, false, sensors );
    }else{
      // if disabled
      jetiEx.SetSensorActive( ID_V1+i, false, sensors );
      jetiEx.SetSensorActive( ID_A1+i, false, sensors );
      jetiEx.SetSensorActive( ID_C1+i, false, sensors );
    }
  }

  jetiEx.SetDeviceId( 0x76, 0x32 );

  jetiEx.Start( "VarioGPS", sensors, JetiExProtocol::SERIAL2 );
}

void loop()
{ 
  static unsigned long lastTime = 0;
  static long startAltitude = 0;
  static long uRelAltitude = 0;
  static long uAbsAltitude = 0;

  if((millis() - lastTime) > MEASURING_INTERVAL){
    
    if(pressureSensor.type != unknown){
      static bool setStartAltitude = false;
      static float lastVariofilter = 0;
      static long lastAltitude = 0;
      long curAltitude;
      long uPressure;
      int uTemperature;
      long uVario;
      int uHumidity;
      
      // Read sensormodule values
      switch (pressureSensor.type){
        #ifdef SUPPORT_MS5611_LPS  
        case MS5611_:
          uPressure = ms5611.readPressure(true); // In Pascal (100 Pa = 1 hPa = 1 mbar)
          curAltitude = ms5611.getAltitude(uPressure, 101325) * 100; // In Centimeter
          uTemperature = ms5611.readTemperature(true) * 10; // In Celsius ( x10 for one decimal)
          break;
        case LPS_:
          uPressure = lps.readPressureMillibars(); // In Pascal (100 Pa = 1 hPa = 1 mbar)
          curAltitude = lps.pressureToAltitudeMeters(uPressure) * 100; // In Centimeter
          uTemperature = lps.readTemperatureC() * 10; // In Celsius ( x10 for one decimal)
          break;
        #endif
        #ifdef SUPPORT_BMx280
        default:
          uPressure = boschPressureSensor.readPressure(); // In Pascal (100 Pa = 1 hPa = 1 mbar)
          curAltitude = boschPressureSensor.readAltitude() * 100; // In Centimeter
          uTemperature = boschPressureSensor.readTemperature() * 10; // In Celsius ( x10 for one decimal)
          if(pressureSensor.type == BME280){
            jetiEx.SetSensorValue( ID_HUMIDITY, boschPressureSensor.readHumidity() * 10 ); // In %rH
          }
          break;
        #endif
      }

      if (!setStartAltitude) {
        // Set start-altitude in sensor-start
        setStartAltitude = true;
        startAltitude = curAltitude;
        lastAltitude = curAltitude;
      }else{
        // Altitude
        uRelAltitude = (curAltitude - startAltitude) / 10;
        uAbsAltitude = curAltitude / 100;
      }
       
      uVario = (curAltitude - lastAltitude) * (1000 / float(millis() - lastTime));
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

      #ifdef UNIT_US
        // EU to US conversions
        // ft/s = m/s / 0.3048
        // inHG = hPa * 0,029529983071445
        // ft = m / 0.3048
        uVario /= 0.3048;
        uPressure *= 0.029529983071445;
        uTemperature = uTemperature * 1.8 + 320;
      #endif
      jetiEx.SetSensorValue( ID_VARIO, uVario );
      jetiEx.SetSensorValue( ID_PRESSURE, uPressure );
      jetiEx.SetSensorValue( ID_TEMPERATURE, uTemperature );
      
    }

    lastTime = millis();

    // analog input
    for(uint8_t i=0; i < MAX_ANALOG_INPUTS; i++){
      if (analogInputMode[i] == voltage){
        // Voltage
        jetiEx.SetSensorValue( ID_V1+i, (analogRead(analogInputPin[i])/1023.0)*V_REF*factorVoltageDivider[i]/10);
      }else if(analogInputMode[i] > voltage){
        // Current
        uint16_t ACSoffset;
    
        if (analogInputMode[i] > ACS758_200B){
          ACSoffset = ACS_U_offset;
        }else{
          ACSoffset = ACS_B_offset;
        }
  
        float mVanalogIn = (analogRead(analogInputPin[i]) / 1023.0) * V_REF; // mV 
        float cuAmp = (mVanalogIn - ACSoffset) / ACS_mVperAmp[analogInputMode[i]-2]; 
 
        jetiEx.SetSensorValue( ID_A1+i, cuAmp*10);

        //Power consumption
        powerConsumption[i] += cuAmp/timefactorPowerConsumption;
        jetiEx.SetSensorValue( ID_C1+i, powerConsumption[i]);
      }
    }
    
  }

  #ifdef SUPPORT_GPS
  if(gpsSettings.mode != GPS_disabled){

    static int homeSetCount = 0;
    static float home_lat;
    static float home_lon;
    static float last_lat;
    static float last_lon;
    static long lastAbsAltitude = 0;
    static unsigned long tripDist;
    unsigned long distToHome;

    // read data from GPS
    while(gpsSerial.available() )
    {
      char c = gpsSerial.read();
      if(gps.encode(c)){
        break;
      }else{
        return;
      }
    }
  

    if (gps.location.isValid() && gps.location.age() < 2000) { // if Fix

      jetiEx.SetSensorValueGPS( ID_GPSLAT, false, gps.location.lat() );
      jetiEx.SetSensorValueGPS( ID_GPSLON, true, gps.location.lng() );

      // Altitude
      uAbsAltitude = gps.altitude.meters();
      
      #ifdef UNIT_US
        jetiEx.SetSensorValue( ID_GPSSPEED, gps.speed.mph() );
      #else
        jetiEx.SetSensorValue( ID_GPSSPEED, gps.speed.kmph() );
      #endif
      
      jetiEx.SetSensorValue( ID_HEADING, gps.course.deg() );
 
      if (homeSetCount < 3000) {  // set home position
        ++homeSetCount;
        home_lat = gps.location.lat();
        home_lon = gps.location.lng();
        last_lat = home_lat;
        last_lon = home_lon;
        lastAbsAltitude = gps.altitude.meters();
        tripDist = 0;
        if(pressureSensor.type == unknown){
          startAltitude = gps.altitude.meters();
        }
        
      }else{
        
        // Rel. Altitude
        if(pressureSensor.type == unknown){
          uRelAltitude = (uAbsAltitude - startAltitude)*10;
        }
      
        // Distance to model
        distToHome = gps.distanceBetween(
                                      gps.location.lat(),
                                      gps.location.lng(),
                                      home_lat,
                                      home_lon);
        if(gpsSettings.distance3D){
          distToHome = sqrt(pow(uRelAltitude/10,2) + pow(distToHome,2));
        }
  
        // Course from home to model
        jetiEx.SetSensorValue( ID_COURSE, gps.courseTo(home_lat,home_lon,gps.location.lat(),gps.location.lng()));

        // Trip distance
        float distLast = gps.distanceBetween(
                                      gps.location.lat(),
                                      gps.location.lng(),
                                      last_lat,
                                      last_lon);
        if(gpsSettings.distance3D){
          distLast = sqrt(pow(uAbsAltitude-lastAbsAltitude,2) + pow(distLast,2));
          lastAbsAltitude = uAbsAltitude;
        }
        tripDist += distLast;
        last_lat = gps.location.lat();
        last_lon = gps.location.lng();
      }
    
    } else { // If Fix end
      jetiEx.SetSensorValueGPS( ID_GPSLAT, false, 0 );
      jetiEx.SetSensorValueGPS( ID_GPSLON, true, 0 );
      if(pressureSensor.type == unknown){
        uRelAltitude = 0;
      }
      uAbsAltitude = 0;
      distToHome = 0;
      jetiEx.SetSensorValue( ID_COURSE, 0 );
      jetiEx.SetSensorValue( ID_GPSSPEED, 0 );
      jetiEx.SetSensorValue( ID_HEADING, 0 );
    }
    
    jetiEx.SetSensorValue( ID_SATS, gps.satellites.value() );
    jetiEx.SetSensorValue( ID_HDOP, gps.hdop.value());
    #ifndef UNIT_US
      //EU units
      jetiEx.SetSensorValue( ID_TRIP, tripDist/10 );
      jetiEx.SetSensorValue( ID_DIST, distToHome );
    #endif
    #ifdef UNIT_US
      //US units
      jetiEx.SetSensorValue( ID_TRIP, tripDist*0.06213 );
      jetiEx.SetSensorValue( ID_DIST, distToHome*3.2808399);
    #endif

  }
  #endif

  #ifdef UNIT_US
    // EU to US conversions
    // ft/s = m/s / 0.3048
    // inHG = hPa * 0,029529983071445
    // ft = m / 0.3048
    uRelAltitude /= 0.3048;
    uAbsAltitude /= 0.3048;
  #endif
  
  jetiEx.SetSensorValue( ID_ALTREL, uRelAltitude );
  jetiEx.SetSensorValue( ID_ALTABS, uAbsAltitude );

  HandleMenu();
  jetiEx.DoJetiSend();
}
