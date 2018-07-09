/*
  ------------------------------------------------------------------
                    Jeti VarioGPS Sensor
  ------------------------------------------------------------------
            *** Universeller Jeti Telemetrie Sensor ***
  Vario, GPS, Strom/Spannung, Empfängerspannungen, Temperaturmessung

*/
#define VARIOGPS_VERSION "Version V2.3"
/*

  ******************************************************************
  Versionen:
  V2.3    09.07.18  MPXV7002/MPXV5004 für Air-Speed wird unterstützt 
                    TEK (Total Energie Kompensation) mit Air-Speed oder GPS-Speed (basierend auf Code von Rainer Stransky)
                    EX-Bus mit 125kbaud, Lib (v0.95) von Bernd Wokoeck
                    Stromsensor per JetiBox kalibrierbar (reset offset) 
                    separate Datei für Einstellungen (settings.h), Compillerwahrnung bei inkompatibilität der Softwareoptionen
                    Bezeichnungen der Telemetriewerte angepasst, damit diese vom Jeti Sender bestmöglich übersetzt werden
                    Fehler behoben: STATIC Variablen entfehrnt, konnte in einigen Fällen zu RAM-Problemen führen und ergab eine fehlerhafte Funktion 
                    Fehler behoben: 2D/3D GPS Distanz in den Einstellungen war verkehrt       
  V2.2.1  26.03.18  Bugfix bei float<->int casting Smoothing Factor (by RS)
  V2.2    15.02.18  Vario Tiefpass mit nur einem Smoothing Factor (by RS)
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
  - Barometer Sensoren: BMP280, BME280, MS5611, LPSxxx
  - Airspeed Sensoren: MPXV7002, MPXV5004
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
  - Vario Filterparameter: Smoothing und Deadzone
  - Stromsensor für Hauptantrieb
  - Einstellung Reset der Kapazität:
        STARTUP(Wert ist nach jedem Einschalten auf 0)
        AUTO(Wert wird gespeichert und erst zurückgesetzt wenn ein geladener Akku angeschlossen wird)
        MANUAL(Wert muss manuell per Jetibox zurückgesetzt werden mit RESET OFFSET)
  - Rx1, Rx2 Empfängerspannungsmessung aktivieren
  - Temperaturmessung aktivieren

*/

#include "settings.h"

// Check supply voltage
#if V_REF < 1800 || V_REF > 5500
  #error unsupported supply voltage
#endif

// Check airspeed-sensor supply voltage
#if defined(SUPPORT_MPXV7002_MPXV5004) && V_REF < 4750 
  #undef SUPPORT_MPXV7002_MPXV5004
  #warning Supply voltage is lower than 4750mV, Airspeed sensor (MPXV7002 / MPXV5004) is disabled
#endif

// Check the compatibility of EX-Bus and GPS sensor
#if defined(SUPPORT_EX_BUS) && defined(SUPPORT_GPS)
  #undef SUPPORT_GPS
  #warning GPS sensor is not compatible with EX-Bus protocol, GPS sensor is disabled
#endif

// Check speed sensor is available for TEC
#if defined(SUPPORT_TEC) && !defined(SUPPORT_MPXV7002_MPXV5004) && !defined(SUPPORT_GPS)
  #undef SUPPORT_TEC
  #warning no speed sensor available, TEC is disabled
#endif

// Check pressure sensor is available for TEC
#if defined(SUPPORT_TEC) && !defined(SUPPORT_BMx280) && !defined(SUPPORT_MS5611) && !defined(SUPPORT_LPS)
  #undef SUPPORT_TEC
  #warning no pressure sensor available, TEC is disabled
#endif

#ifdef SUPPORT_EX_BUS
  #include "JetiExBusProtocol.h"
  JetiExBusProtocol jetiEx;
#else
  #include <JetiExSerial.h>
  #include <JetiExProtocol.h>
  JetiExProtocol jetiEx;
#endif

#include <EEPROM.h>
#include <Wire.h>
#include "defaults.h"

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

#ifdef SUPPORT_MS5611
  #include <MS5611.h>
  MS5611 ms5611;
#endif

#ifdef SUPPORT_LPS
  #include <LPS.h>
  LPS lps;
#endif

struct {
  uint8_t mode = DEFAULT_GPS_MODE;
  bool distance3D = DEFAULT_GPS_3D_DISTANCE;
} gpsSettings;

struct {
  uint8_t type = unknown;
  float smoothingValue;
  uint8_t deadzone;
} pressureSensor;

// set defaults
uint8_t currentSensor = DEFAULT_CURRENT_SENSOR;
uint8_t capacityMode = DEFAULT_CAPACITY_MODE;
bool enableRx1 = DEFAULT_ENABLE_Rx1;
bool enableRx2 = DEFAULT_ENABLE_Rx2;
bool enableExtTemp = DEFAULT_ENABLE_EXT_TEMP;
uint8_t TECmode = DEFAULT_TEC_MODE;
uint8_t airSpeedSensor = DEFAULT_AIRSPEED_SENSOR;

// Main drive variables
unsigned long lastTime = 0;
unsigned long lastTimeCapacity = 0;
float cuAmp;
int ampOffsetCalibration = 0;
float capacityConsumption = 0;
const float timefactorCapacityConsumption = (1000.0/MEASURING_INTERVAL*60*60)/1000;
const float factorVoltageDivider[] = { float(voltageInputR1[0]+voltageInputR2[0])/voltageInputR2[0],
                                       float(voltageInputR1[1]+voltageInputR2[1])/voltageInputR2[1],
                                       float(voltageInputR1[2]+voltageInputR2[2])/voltageInputR2[2],
                                       float(voltageInputR1[3]+voltageInputR2[3])/voltageInputR2[3],
                                       float(voltageInputR1[4]+voltageInputR2[4])/voltageInputR2[4],
                                       float(voltageInputR1[5]+voltageInputR2[5])/voltageInputR2[5],
                                       float(voltageInputR1[6]+voltageInputR2[6])/voltageInputR2[6],
                                       float(voltageInputR1[7]+voltageInputR2[7])/voltageInputR2[7]  };

// pressure sensor variables
long startAltitude = 0;
//bool setStartAltitude = false;
long uRelAltitude = 0;
long uAbsAltitude = 0;
long uPressure = PRESSURE_SEALEVEL;
int uTemperature = 20;
float lastVariofilter = 0;
long lastAltitude = 0;

// airspeed variables
int refAirspeedPressure;
int uAirSpeed = 0;
int lastAirSpeed = 0;

// TEC variables
unsigned long dT = 0;
float dV;
float uSpeedMS;
long  lastTimeSpeed = 0;
float lastGPSspeedMS = 0;

// GPS variables
int homeSetCount = 0;
float home_lat;
float home_lon;
float last_lat;
float last_lon;
long lastAbsAltitude = 0;
unsigned long tripDist;


// Restart CPU
void(* resetCPU) (void) = 0;

#include "HandleMenu.h"

// read mV from voltage sensor
long readAnalog_mV(uint8_t sensorVolt, uint8_t pin){
  return (analogRead(pin)/1024.0)*V_REF*factorVoltageDivider[sensorVolt];
}

// get voltage sensor type
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
  return 0;
}

void setup()
{
  // identify pressure sensor
  #ifdef SUPPORT_BMx280
    pressureSensor.type = boschPressureSensor.begin(0x76);
    if(pressureSensor.type == unknown){
      pressureSensor.type = boschPressureSensor.begin(0x77);
    }
  #endif
  #if defined(SUPPORT_MS5611) || defined(SUPPORT_LPS)
    if(pressureSensor.type == unknown){
      #ifdef SUPPORT_LPS
      if (lps.init()) {
        Wire.begin();
        lps.enableDefault();
        pressureSensor.type = LPS_;
      } else {
      #endif
        Wire.beginTransmission(MS5611_ADDRESS); // if no Bosch sensor, check if return an ACK on MS5611 address
        if (Wire.endTransmission() == 0) {
          ms5611.begin(MS5611_ULTRA_HIGH_RES);
          pressureSensor.type = MS5611_;
        }
      #ifdef SUPPORT_LPS
      }
      #endif
    }
  #endif

  #if defined(SUPPORT_BMx280) || defined(SUPPORT_MS5611) || defined(SUPPORT_LPS)
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
  if (EEPROM.read(P_GPS_MODE) != 0xFF) {
    gpsSettings.mode = EEPROM.read(P_GPS_MODE);
  }
  if (EEPROM.read(P_GPS_3D) != 0xFF) {
    gpsSettings.distance3D = EEPROM.read(P_GPS_3D);
  }
  #endif

  #ifdef SUPPORT_MAIN_DRIVE
  if (EEPROM.read(P_CURRENT_SENSOR) != 0xFF) {
    currentSensor = EEPROM.read(P_CURRENT_SENSOR);
  }
  if (EEPROM.read(P_CURRENT_CALIBRATION) != 0xFF) {
    ampOffsetCalibration = EEPROM.read(P_CURRENT_CALIBRATION)-127;
  }
  if (EEPROM.read(P_CAPACITY_MODE) != 0xFF) {
    capacityMode = EEPROM.read(P_CAPACITY_MODE);
  }
  #endif

  #ifdef SUPPORT_RX_VOLTAGE
  if (EEPROM.read(P_ENABLE_RX1) != 0xFF) {
    enableRx1 = EEPROM.read(P_ENABLE_RX1);
  }
  if (EEPROM.read(P_ENABLE_RX2) != 0xFF) {
    enableRx2 = EEPROM.read(P_ENABLE_RX2);
  }
  #endif

  #ifdef SUPPORT_EXT_TEMP
  if (EEPROM.read(P_ENABLE_TEMP) != 0xFF) {
    enableExtTemp = EEPROM.read(P_ENABLE_TEMP);
  }
  #endif

  if (EEPROM.read(P_VARIO_SMOOTHING) != 0xFF) {
    pressureSensor.smoothingValue = (float)EEPROM.read(P_VARIO_SMOOTHING) / 100;
  }
  if (EEPROM.read(P_VARIO_DEADZONE) != 0xFF) {
    pressureSensor.deadzone = EEPROM.read(P_VARIO_DEADZONE);
  }

  #ifdef SUPPORT_MPXV7002_MPXV5004
  if (EEPROM.read(P_AIRSPEED_SENSOR) != 0xFF) {
    airSpeedSensor = EEPROM.read(P_AIRSPEED_SENSOR);
  }
  #endif
  
  #ifdef SUPPORT_TEC
  if (EEPROM.read(P_TEC_MODE) != 0xFF) {
    TECmode = EEPROM.read(P_TEC_MODE);
  }
  #endif
  
  #ifdef SUPPORT_GPS
    // init GPS
    gpsSerial.begin(GPSBaud);
  #endif

  #ifdef SUPPORT_MPXV7002_MPXV5004
  // init airspeed sensor
  if(airSpeedSensor){
    //delay(50);
    refAirspeedPressure = analogRead(AIRSPEED_PIN);
  }
  #endif


  // Setup telemetry sensors
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
    jetiEx.SetSensorActive( ID_AZIMUTH, false, sensors );
    jetiEx.SetSensorActive( ID_COURSE, false, sensors );
    jetiEx.SetSensorActive( ID_SATS, false, sensors );
    jetiEx.SetSensorActive( ID_HDOP, false, sensors );
  }

  if(!airSpeedSensor){
    jetiEx.SetSensorActive( ID_AIRSPEED, false, sensors );
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
      EEPROM.get(P_CAPACITY_VALUE, capacityConsumption);
      if(capacityMode == automatic){
        float cuVolt = readAnalog_mV(getVoltageSensorTyp(),VOLTAGE_PIN)/1000.0;
        float oldVolt;
        EEPROM.get(P_VOLTAGE_VALUE, oldVolt);
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
  

  #ifdef SUPPORT_EX_BUS
  jetiEx.Start( "VarioGPS", sensors, 0 );
  #else
  jetiEx.Start( "VarioGPS", sensors, JetiExProtocol::SERIAL2 );
  #endif

}

void loop()
{
  #ifdef SUPPORT_EX_BUS
  //if(jetiEx.IsBusReleased()){
  #endif
  
  if((millis() - lastTime) > MEASURING_INTERVAL){

    #ifdef SUPPORT_MPXV7002_MPXV5004
    if(airSpeedSensor){
      // get air speed from MPXV7002/MPXV5004
      // based on code from johnlenfr, http://johnlenfr.1s.fr
      int airSpeedPressure = analogRead(AIRSPEED_PIN);
      if (airSpeedPressure < refAirspeedPressure) airSpeedPressure = refAirspeedPressure;
      
      float pitotpressure = 5000.0 * ((airSpeedPressure - refAirspeedPressure) / 1024.0f) + uPressure;    // differential pressure in Pa, 1 V/kPa, max 3920 Pa
      float density = (uPressure * DRY_AIR_MOLAR_MASS) / ((uTemperature + 273.16) * UNIVERSAL_GAS_CONSTANT);
      uAirSpeed = sqrt ((2 * (pitotpressure - uPressure)) / density);
      
      // IIR Low Pass Filter
      uAirSpeed = uAirSpeed + AIRSPEED_SMOOTHING * (lastAirSpeed - uAirSpeed);
  
      #ifdef SUPPORT_TEC
      if(TECmode == TEC_airSpeed){
        uSpeedMS = uAirSpeed;
        dV = uAirSpeed - lastAirSpeed;     // delta speed in m/s
      }
      #endif
      
      lastAirSpeed = uAirSpeed;

      #ifdef UNIT_US
        jetiEx.SetSensorValue( ID_AIRSPEED, uAirSpeed*2.23694 );    // speed in mph
      #else
        jetiEx.SetSensorValue( ID_AIRSPEED, uAirSpeed*3.6 );        // speed in km/h
      #endif
    }
    #endif

    #if defined(SUPPORT_BMx280) || defined(SUPPORT_MS5611) || defined(SUPPORT_LPS)
    if(pressureSensor.type != unknown){
      long curAltitude;
      long uVario;

      // Read sensormodule values
      switch (pressureSensor.type){
        #ifdef SUPPORT_MS5611
        case MS5611_:
          uPressure = ms5611.readPressure(true); // In Pascal (100 Pa = 1 hPa = 1 mbar)
          curAltitude = ms5611.getAltitude(uPressure, PRESSURE_SEALEVEL) * 100; // In Centimeter
          uTemperature = ms5611.readTemperature(true); // In Celsius
          break;
        #endif
        #ifdef SUPPORT_LPS
        case LPS_:
          uPressure = lps.readPressureMillibars(); // In Pascal (100 Pa = 1 hPa = 1 mbar)
          curAltitude = lps.pressureToAltitudeMeters(uPressure) * 100; // In Centimeter
          uTemperature = lps.readTemperatureC(); // In Celsius
          break;
        #endif
        #ifdef SUPPORT_BMx280
        default:
          uPressure = boschPressureSensor.readPressure(); // In Pascal (100 Pa = 1 hPa = 1 mbar)
          curAltitude = boschPressureSensor.readAltitude() * 100; // In Centimeter
          uTemperature = boschPressureSensor.readTemperature(); // In Celsius
          if(pressureSensor.type == BME280){
            jetiEx.SetSensorValue( ID_HUMIDITY, boschPressureSensor.readHumidity() * 10 ); // In %rH
          }
          break;
        #endif
      }

      //if (!setStartAltitude) {
      if (startAltitude == 0) {
        // Set start-altitude in sensor-start
        //setStartAltitude = true;
        startAltitude = curAltitude;
        lastAltitude = curAltitude;
      }else{
        // Altitude
        uRelAltitude = (curAltitude - startAltitude) / 10;
        uAbsAltitude = curAltitude / 100;
      }

      // Vario calculation
      unsigned long dTvario = millis() - lastTime;     // delta time in ms
      uVario = (curAltitude - lastAltitude) * (1000.0 / dTvario);
      lastAltitude = curAltitude;      

      // TEC (Total energie compensation)  
      // based on code from Rainer Stransky
      // see: http://www.how2soar.de/images/H2S_media/02_pdf/TE-Vario_im_Stroemungsfeld.pdf  
      #ifdef SUPPORT_TEC
     
      if(TECmode == TEC_airSpeed){
        dT = dTvario;
      }

      if (dT != 0 && dT < 3000) { // avoid divison by zero
        long tvario = uSpeedMS  * 10194 * dV / dT;
        uVario += tvario;
      }
      #endif
    
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
      
      // Vario dead zone filter
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
        jetiEx.SetSensorValue( ID_VARIO, uVario /= 0.3048);                 // ft/s
        jetiEx.SetSensorValue( ID_PRESSURE, uPressure * 0.029529983071445 );  // inHG
        jetiEx.SetSensorValue( ID_TEMPERATURE, (uTemperature * 1.8 + 320) * 10);    // °F
      #else
        jetiEx.SetSensorValue( ID_VARIO, uVario );
        jetiEx.SetSensorValue( ID_PRESSURE, uPressure );
        jetiEx.SetSensorValue( ID_TEMPERATURE, uTemperature * 10 );
      #endif
      
    }
    #endif

    lastTime = millis();

    // analog input
    #ifdef SUPPORT_MAIN_DRIVE
    if(currentSensor){
      
      // Voltage calculation
      float cuVolt = readAnalog_mV(getVoltageSensorTyp(),VOLTAGE_PIN)/1000.0;
      jetiEx.SetSensorValue( ID_VOLTAGE, cuVolt*10);

      // Current calculation
      uint16_t ampOffset;

      if (currentSensor <= APM25_A){
        ampOffset = Atto_APM_offset;
      }else if (currentSensor > ACS758_200B){
        ampOffset = ACS_U_offset;
      }else{
        ampOffset = ACS_B_offset;
      }

      ampOffset += ampOffsetCalibration;

      float mVanalogIn = (analogRead(CURRENT_PIN) / 1024.0) * V_REF; // mV
      cuAmp = (mVanalogIn - ampOffset) / mVperAmp[currentSensor-1];
      if (currentSensor > APM25_A){
        cuAmp *= 5000.0/V_REF;
      }

      jetiEx.SetSensorValue( ID_CURRENT, cuAmp*10);

      // Capacity consumption calculation
      capacityConsumption += cuAmp/timefactorCapacityConsumption;
      jetiEx.SetSensorValue( ID_CAPACITY, capacityConsumption);

      // save capacity and voltage to eeprom
      if(capacityMode > startup && (millis() - lastTimeCapacity) > CAPACITY_SAVE_INTERVAL){
        if(cuAmp <= MAX_CUR_TO_SAVE_CAPACITY){
          EEPROM.put(P_CAPACITY_VALUE, capacityConsumption);
          EEPROM.put(P_VOLTAGE_VALUE, cuVolt);
        }
        lastTimeCapacity = millis();
      }

      // Power calculation
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

    // temperature calculation
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

      #ifdef SUPPORT_TEC
      if(TECmode == TEC_GPS){
        if (gps.speed.isUpdated()) {
          uSpeedMS = gps.speed.kmph() / 3.6;
          dV = uSpeedMS - lastGPSspeedMS;     // delta speed in m/s
          lastGPSspeedMS = uSpeedMS;
          long uTimeSpeed = millis();
          dT = uTimeSpeed - lastTimeSpeed; // dT in ms
          lastTimeSpeed = uTimeSpeed;
        }
        
      }
      #endif

      #ifdef UNIT_US
        jetiEx.SetSensorValue( ID_GPSSPEED, gps.speed.mph() );
      #else
        jetiEx.SetSensorValue( ID_GPSSPEED, gps.speed.kmph() );
      #endif

      

      jetiEx.SetSensorValue( ID_AZIMUTH, gps.course.deg() );

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
      jetiEx.SetSensorValue( ID_AZIMUTH, 0 );
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

  #ifdef SUPPORT_EX_BUS
  //}
  jetiEx.DoJetiExBus();
  #else
  jetiEx.DoJetiSend();
  #endif
  
}
