/*
  ------------------------------------------------------------------
                    Jeti VarioGPS Sensor
  ------------------------------------------------------------------
            *** Universeller Jeti Telemetrie Sensor ***
  Vario, GPS, Strom/Spannung, Empfängerspannungen, Temperaturmessung
  
*/
#define VARIOGPS_VERSION "Version V2.2"
/*

  ******************************************************************
  Versionen:
  V2.2    16.02.18  Vario Tiefpass mit nur einem Smoothing Factor (by RS)
                    Jeder Sensor kann mit #define deaktiviert werden
  V2.1.1  13.01.18  kleine Fehlerbehebung mit libraries
  V2.1    23.12.17  Analog Messeingänge stark überarbeitet, NTC-Temperaturmessung hinzugefügt, 
                    startup-/auto-/maual-reset für Kapazitätsanzeige, SRAM-Speicheroptimierung    
  V2.0.2  03.12.17  Fehler in GPS Trip behoben
  V2.0.1  21.11.17  Fehler bei Spannungsmessung behoben
  V2.0    20.11.17  GPS Trip[km] und verbrauchte Kapazität[mAh] eingebaut, Stromsensoren ACS712 eingebaut
  V1.9    17.11.17  ACSxxx Stromsensoren eingebaut
  V1.8    17.11.17  Luftdrucksensor MS5611/LPS werden unterstützt
  V1.7.2  15.11.17  Fehlerbehebung mit BME280
  V1.7.1  15.11.17  Speicheroptimierung, kleinere Fehler behoben
  V1.7    12.11.17  Empfängerspannungen können gemessen werden
  V1.6    05.11.17  Luftdrucksensoren BMP085/BMP180/BMP280/BME280 eingebaut, und zu VarioGPS umbenannt
  V1.5    05.11.17  Code von RC-Thoughts(Jeti_GPS-Sensor) übernommen




  ******************************************************************
  Unterstützte Hardware:
  
  - Arduino Pro Mini 3.3V-8Mhz/5V-16Mhz
  - GPS-Modul mit NMEA Protokoll und UART@9600baud
  - Luftdrucksensoren: BMP280, BME280, MS5611, LPS 
  - Stromsensoren @3.3V/5V Betriebsspannung:        AttoPilot Module @3.3V: 45A/13.6V - 90A/50V - 180A/50V (@5V: 45A/20.6V - 90A/60V - 180A/60V)
                                                    APM2.5 PowerModul @5V: 90A/50V (@3.3V: 58A/33.4V)
                                                    ACS758_50B, ACS758_100B, ACS758_150B, ACS758_200B, ACS758_50U, ACS758_100U, ACS758_150U, ACS758_200U
  - zusätzliche Stromsensoren @5V Betriebsspannung: ACS712_05, ACS712_20, ACS712_30
  


  ******************************************************************
  Anzeige:
  
  Nur mit Luftdrucksensor werden die Werte angezeigt:
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
  - Distanz vom Modell zum Startpunkt (2D oder 3D)
  - zurückgelegte Strecke (Trip)
  - Flugrichtung (Heading)
  - Kurs vom Modell zum Startpunkt
  - Anzahl Satelliten
  - HDOP (Horizontaler Faktor der Positionsgenauigkeit)
  - Luftdruck
  - Temperatur
  - Luftfeuchtigkeit 

  An den Analogeingängen sind folgende Messungen möglich:
  - Strom- und Spannung für Hauptantrieb mit verbrauchter Kapazität[mAh] und Leistung[W]
  - 2x Empfängerspannung
  - Temperatur mit NTC-Wiederstand von -55 bis +155°C
  
  Folgende Einstellungen können per Jetibox vorgenommen werden:
  - GPS: deaktiviert, Basic oder Extended
  - GPS Distanz: 2D oder 3D
  - Vario Filterparameter X, Y und Deadzone
  - Stromsensor für Hauptantrieb 
  - Einstellung Reset der Kapazität: 
        STARTUP(Wert ist nach jedem Einschalten auf 0)
        AUTO(Wert wird gespeichert und erst zurückgesetzt wenn ein geladener Akku angeschlossen wird)
        MANUAL(Wert muss manuell per Jetibox zurückgesetzt werden mit RESET OFFSET)
  - Rx1, Rx2 Empfängerspannungsmessung aktivieren
  - Temperaturmessung aktivieren
   
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

#if V_REF < 1800 || V_REF > 5500
  #error unsupported supply voltage
#endif

#define MEASURING_INTERVAL        180         //ms 
#define EEPROM_ADRESS_CAPACITY    20

struct {
  uint8_t mode = DEFAULT_GPS_MODE;
  bool distance3D = DEFAULT_GPS_3D_DISTANCE;
} gpsSettings;

struct {
  uint8_t type = unknown;
  float smoothingValue;
  long deadzone;
} pressureSensor;


unsigned long lastTime = 0;
unsigned long lastTimeCapacity = 0;
uint8_t currentSensor = DEFAULT_CURRENT_SENSOR;
uint8_t capacityMode = DEFAULT_CAPACITY_MODE;
bool enableRx1 = DEFAULT_ENABLE_Rx1;
bool enableRx2 = DEFAULT_ENABLE_Rx2;
bool enableExtTemp = DEFAULT_ENABLE_EXT_TEMP;

const float factorVoltageDivider[] = { float(voltageInputR1[0]+voltageInputR2[0])/voltageInputR2[0],
                                       float(voltageInputR1[1]+voltageInputR2[1])/voltageInputR2[1],
                                       float(voltageInputR1[2]+voltageInputR2[2])/voltageInputR2[2],
                                       float(voltageInputR1[3]+voltageInputR2[3])/voltageInputR2[3],
                                       float(voltageInputR1[4]+voltageInputR2[4])/voltageInputR2[4],
                                       float(voltageInputR1[5]+voltageInputR2[5])/voltageInputR2[5],
                                       float(voltageInputR1[6]+voltageInputR2[6])/voltageInputR2[6],
                                       float(voltageInputR1[7]+voltageInputR2[7])/voltageInputR2[7]  };

const float timefactorCapacityConsumption = (1000.0/MEASURING_INTERVAL*60*60)/1000;
float capacityConsumption = 0;

// Restart by user
void(* resetFunc) (void) = 0;

#include "HandleMenu.h"

long readAnalog_mV(uint8_t sensorVolt, uint8_t pin){
  return (analogRead(pin)/1023.0)*V_REF*factorVoltageDivider[sensorVolt];
}

uint8_t getVoltageSensorTyp(){
  if (currentSensor <= APM25_A){
    return currentSensor-1;
  #if V_REF >= 4500
  }else if(currentSensor >= ACS712_05 && currentSensor <= ACS712_30){
    return ACS712_voltage;
  #endif
  }else if(currentSensor >= ACS758_50B){
    return ACS758_voltage;
  }
}

void setup()
{
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

  #ifdef SUPPORT_BMx280 || SUPPORT_MS5611_LPS
  switch (pressureSensor.type){
    case BMP280:
      pressureSensor.smoothingValue = BMx280_SMOOTHING;
      pressureSensor.deadzone = BMx280_DEADZONE;
      break;
    case BME280:
      pressureSensor.smoothingValue = BMx280_SMOOTHING;
      pressureSensor.deadzone = BMx280_DEADZONE;
      break;
    case MS5611_ :
      pressureSensor.smoothingValue = MS5611_SMOOTHING;
      pressureSensor.deadzone = MS5611_DEADZONE;
      break;
    case LPS_ :
      pressureSensor.smoothingValue = LPS_SMOOTHING;
      pressureSensor.deadzone = LPS_DEADZONE;
      break;
  }
  #endif
 
  // read settings from eeprom 
  #ifdef SUPPORT_GPS
  if (EEPROM.read(1) != 0xFF) {
    gpsSettings.mode = EEPROM.read(1);
  }
  if (EEPROM.read(2) != 0xFF) {
    gpsSettings.distance3D = EEPROM.read(2);
  }
  #endif

  #ifdef SUPPORT_MAIN_DRIVE
  if (EEPROM.read(3) != 0xFF) {
    currentSensor = EEPROM.read(3);
  }
  if (EEPROM.read(5) != 0xFF) {
    capacityMode = EEPROM.read(5);
  }
  #endif

  #ifdef SUPPORT_RX_VOLTAGE
  if (EEPROM.read(6) != 0xFF) {
    enableRx1 = EEPROM.read(6);
  }
  if (EEPROM.read(7) != 0xFF) {
    enableRx2 = EEPROM.read(7);
  }
  #endif

  #ifdef SUPPORT_EXT_TEMP
  if (EEPROM.read(8) != 0xFF) {
    enableExtTemp = EEPROM.read(8);
  }
  #endif
  
  if (EEPROM.read(10) != 0xFF) {
    pressureSensor.smoothingValue = (float)EEPROM.read(10) / 100;
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

  if(currentSensor == mainDrive_disabled){
    jetiEx.SetSensorActive( ID_VOLTAGE, false, sensors );
    jetiEx.SetSensorActive( ID_CURRENT, false, sensors );
    jetiEx.SetSensorActive( ID_CAPACITY, false, sensors );
    jetiEx.SetSensorActive( ID_POWER, false, sensors );
    #ifdef SUPPORT_MAIN_DRIVE
  }else{
    if(capacityMode > startup){
      // read capacity from eeprom
      int eeAddress = EEPROM_ADRESS_CAPACITY;
      EEPROM.get(eeAddress, capacityConsumption);
      if(capacityMode == automatic){
        eeAddress += sizeof(float);
        float cuVolt = readAnalog_mV(getVoltageSensorTyp(),VOLTAGE_PIN)/1000.0;
        float oldVolt;
        EEPROM.get(eeAddress, oldVolt);
        if(cuVolt >= oldVolt * ((100.0+VOLTAGE_DIFFERENCE)/100.0)){
          capacityConsumption = 0;
        }
      }
    }
    #endif
  }

  if(!enableRx1){
    jetiEx.SetSensorActive( ID_RX1_VOLTAGE, false, sensors );
  }

  if(!enableRx2){
    jetiEx.SetSensorActive( ID_RX2_VOLTAGE, false, sensors );
  }

  if(!enableExtTemp){
    jetiEx.SetSensorActive( ID_EXT_TEMP, false, sensors );
  }

  // init Jeti EX Bus
  jetiEx.SetDeviceId( 0x76, 0x32 );
  jetiEx.Start( "VarioGPS", sensors, JetiExProtocol::SERIAL2 );

}

void loop()
{ 
  static long startAltitude = 0;
  static long uRelAltitude = 0;
  static long uAbsAltitude = 0;

  if((millis() - lastTime) > MEASURING_INTERVAL){

    #ifdef SUPPORT_BMx280 || SUPPORT_MS5611_LPS
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
      // IIR Low Pass Filter
      // y[i] := α * x[i] + (1-α) * y[i-1]
      //      := y[i-1] + α * (x[i] - y[i-1])
      // mit α = 1- β
      // y[i] := (1-β) * x[i] + β * y[i-1]
      //      := x[i] - β * x[i] + β * y[i-1]
      //      := x[i] + β (y[i-1] - x[i])
      // see: https://en.wikipedia.org/wiki/Low-pass_filter#Simple_infinite_impulse_response_filter

      uVario = uVario + pressureSensor.smoothingValue * (lastVariofilter - uVario);
      lastVariofilter = uVario;
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
    #endif

    lastTime = millis();

    // analog input
    #ifdef SUPPORT_MAIN_DRIVE
    if(currentSensor){
      // Voltage
      float cuVolt = readAnalog_mV(getVoltageSensorTyp(),VOLTAGE_PIN)/1000.0;
      jetiEx.SetSensorValue( ID_VOLTAGE, cuVolt*10);

      // Current
      uint16_t ampOffset;
  
      if (currentSensor <= APM25_A){
        ampOffset = Atto_APM_offset;
      }else if (currentSensor > ACS758_200B){
        ampOffset = ACS_U_offset;
      }else{
        ampOffset = ACS_B_offset;
      }
  
      float mVanalogIn = (analogRead(CURRENT_PIN) / 1023.0) * V_REF; // mV 
      float cuAmp = (mVanalogIn - ampOffset) / mVperAmp[currentSensor-1]; 
      if (currentSensor > APM25_A){
        cuAmp *= 5000.0/V_REF;
      }

      jetiEx.SetSensorValue( ID_CURRENT, cuAmp*10);

      // Capacity consumption
      capacityConsumption += cuAmp/timefactorCapacityConsumption;
      jetiEx.SetSensorValue( ID_CAPACITY, capacityConsumption);

      // save capacity and voltage to eeprom
      if(capacityMode > startup && (millis() - lastTimeCapacity) > CAPACITY_SAVE_INTERVAL){
        if(cuAmp <= MAX_CUR_TO_SAVE_CAPACITY){
          int eeAddress = EEPROM_ADRESS_CAPACITY;
          EEPROM.put(eeAddress, capacityConsumption);
          eeAddress += sizeof(float);
          EEPROM.put(eeAddress, cuVolt);
        }
        lastTimeCapacity = millis();
      }

      // Power
      jetiEx.SetSensorValue( ID_POWER, cuAmp*cuVolt);
    }
    #endif

    #ifdef SUPPORT_RX_VOLTAGE
    if(enableRx1){
      jetiEx.SetSensorValue( ID_RX1_VOLTAGE, readAnalog_mV(Rx1_voltage,RX1_VOLTAGE_PIN)/10);
    }

    if(enableRx2){
      jetiEx.SetSensorValue( ID_RX2_VOLTAGE, readAnalog_mV(Rx2_voltage,RX2_VOLTAGE_PIN)/10);
    }
    #endif

    #ifdef SUPPORT_EXT_TEMP
    if(enableExtTemp){
      // convert the value to resistance
      float aIn = 1023.0 / analogRead(TEMPERATURE_PIN) - 1.0;
      aIn = SERIESRESISTOR / aIn;

      // convert resistance to temperature
      float steinhart;
      steinhart = aIn / THERMISTORNOMINAL;                // (R/Ro)
      steinhart = log(steinhart);                         // ln(R/Ro)
      steinhart /= BCOEFFICIENT;                          // 1/B * ln(R/Ro)
      steinhart += 1.0 / (TEMPERATURENOMINAL + 273.15);   // + (1/To)
      steinhart = 1.0 / steinhart;                        // Invert
      steinhart -= 273.15 - SELF_HEAT;                    // convert to °C and self-heat compensation 
      
      #ifdef UNIT_US
        // EU to US conversions
        steinhart = steinhart * 1.8 + 320;
      #endif
      
      jetiEx.SetSensorValue( ID_EXT_TEMP, steinhart*10);
    }
    #endif
   
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

  #ifdef SUPPORT_JETIBOX_MENU
  HandleMenu();
  #endif
  jetiEx.DoJetiSend();
}
