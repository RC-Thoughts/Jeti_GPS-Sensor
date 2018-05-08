/*
  -----------------------------------------------------------
            Menu handler for Jetibox
  -----------------------------------------------------------
*/


// Jetibox screen names
enum screenViews {
  aboutScreen,
  resetOffset,
  #ifdef SUPPORT_GPS
  setGpsMode,
  setDistanceMode,
  #endif
  detectedPressureSensor,
  setVarioSmoothingValue,
  setDeadzone,
  #ifdef SUPPORT_MPXV7002_MPXV5004
  setAirSpeedSensor,
  #endif
  #ifdef SUPPORT_TEC
  setTECmode,
  #endif
  #ifdef SUPPORT_MAIN_DRIVE
  setMainDrive,
  setCapacityMode,
  #endif
  #ifdef SUPPORT_RX_VOLTAGE
  enableRx1Voltage,
  enableRx2Voltage,
  #endif
  #ifdef SUPPORT_EXT_TEMP
  enableExternTemp,
  #endif
  saveSettings,
  defaultSettings
};


const char menuText[][17] PROGMEM=
{
  {"VarioGPS Sensor"},
  {"Reset offset"},
  #ifdef SUPPORT_GPS
  {"GPS mode:"},
  {"GPS distance:"},
  #endif
  {"Pressure sensor:"},
  {"Vario smoothing:"},
  {"Vario deadzone:"},
  #ifdef SUPPORT_MPXV7002_MPXV5004
  {"Air speed:"},
  #endif
  #ifdef SUPPORT_TEC
  {"TEC mode:"},
  #endif
  #ifdef SUPPORT_MAIN_DRIVE
  {"Main drive:"},
  {"Capacity reset:"},
  #endif
  #ifdef SUPPORT_RX_VOLTAGE
  {"Rx1 voltage:"},
  {"Rx2 voltage:"},
  #endif
  #ifdef SUPPORT_EXT_TEMP
  {"Ext. Temp:"},
  #endif
  {"Save and restart"},
  {"Load defaults"}
};

const char aboutScreenText[17] PROGMEM= {VARIOGPS_VERSION};

const char setGpsModeText[][10] PROGMEM=
{
  {" Disabled"},
  {" Basic"},
  {" Extended"}
};

const char setDistanceModeText[][4] PROGMEM=
{
  {" 3D"},
  {" 2D"}
};

const char detectedPressureSensorText[][9] PROGMEM=
{
  {" unknown"},
  {" BMP280"},
  {" BME280"},
  {" MS5611"},
  {" LPS"}
};

/*const char setAirSpeedSensorText[][10] PROGMEM=
{
  {" Disabled"},
  {" MPXV7002"}
};*/

const char setTECmodeText[][11] PROGMEM=
{
  {" Disabled"},
  {" Air speed"},
  {" GPS"}
};

const char setMainDriveText[][16] PROGMEM=
{
  {" Disabled"},
  {" AttoPilot 45"},
  {" AttoPilot 90"},
  {" AttoPilot 180"},
  {" APM2.5 90A/50V"},
  #if V_REF > 4500
  {" ACS712-05"},
  {" ACS712-20"},
  {" ACS712-30"},
  #endif
  {" ACS758-50B"},
  {" ACS758-100B"},
  {" ACS758-150B"},
  {" ACS758-200B"},
  {" ACS758-50U"},
  {" ACS758-100U"},
  {" ACS758-150U"},
  {" ACS758-200U"}
};

const char setCapacityModeText[][9] PROGMEM=
{
  {" Startup"},
  {" Auto"},
  {" Manual"}
};

const char enableText[][10] PROGMEM=
{
  {" Disabled"},
  {" Enabled"}
};

// reset capacity and voltage value in eeprom
void resetCapacity(){
  EEPROM.put(P_CAPACITY_VALUE, 0.0f);                 
  EEPROM.put(P_VOLTAGE_VALUE, 0.0f);
}

void HandleMenu()
{
  static int  _nMenu = aboutScreen;
  static bool _bSetDisplay = true;
  static uint32_t LastKey;
  char _bufferLine1[17];
  char _bufferLine2[17];
  uint8_t c = jetiEx.GetJetiboxKey();

  enum
  {
    keyRight       = 0xe0,
    keyLeft        = 0x70,
    keyUp          = 0xd0,
    keyDown        = 0xb0,
    keyUpDown      = 0x90,
    keyLeftRight   = 0x60
  };

  if( c == 0 && !_bSetDisplay) return;

  if( millis() < LastKey )
    return;
  LastKey = millis() + 200;

  startHandleMenu:

  // Right
  if ( c == keyRight && _nMenu < defaultSettings)
  {
    _nMenu++;
    _bSetDisplay = true;
  }

  // Left
  if ( c == keyLeft &&  _nMenu > aboutScreen )
  {
    _nMenu--;
    _bSetDisplay = true;
  }

  // UP
  if ( c == keyUp )
  {
    switch ( _nMenu )
    {
      #ifdef SUPPORT_GPS
      case setGpsMode:
        if(gpsSettings.mode > GPS_disabled){
          gpsSettings.mode--;
        }
        break;
      #endif
      case setVarioSmoothingValue:
        if (pressureSensor.smoothingValue < 1.0) {
          pressureSensor.smoothingValue = (pressureSensor.smoothingValue*100+1)/100;
        }
        break;
      case setDeadzone:
        if (pressureSensor.deadzone < 100) {
          pressureSensor.deadzone++;
        }
        break;
      #ifdef SUPPORT_MPXV7002_MPXV5004
      case setAirSpeedSensor:
        if(airSpeedSensor > airSpeed_disabled){
          airSpeedSensor--;
        }
        break;
      #endif
      #ifdef SUPPORT_TEC
      case setTECmode:
        if(TECmode > TEC_disabled){
          TECmode--;
        }
        //if(TECmode == TEC_airSpeed && airSpeedSensor == airSpeed_disabled)goto startHandleMenu;
        //if(TECmode == TEC_GPS && gpsSettings.mode == GPS_disabled)TECmode = TEC_disabled;
        break;
      #endif
      #ifdef SUPPORT_MAIN_DRIVE
      case setMainDrive:
        if (currentSensor > mainDrive_disabled) {
          currentSensor--;
        }
        break;
      case setCapacityMode:
        if(capacityMode > startup){
          capacityMode--;
        }
        break;
      #endif
    }

    _bSetDisplay = true;
  }

  // DN
  if ( c == keyDown )
  {
    switch ( _nMenu )
    {
      case resetOffset:
        // reset capacity in eeprom
        resetCapacity();
        // calibrate currentoffset
        if(currentSensor){
          ampOffsetCalibration += cuAmp*mVperAmp[currentSensor-1];
          EEPROM.write(P_CURRENT_CALIBRATION,ampOffsetCalibration+127);
        }
        resetCPU();
      #ifdef SUPPORT_GPS
      case setGpsMode:
        if(gpsSettings.mode < GPS_extended){
          gpsSettings.mode++;
        }
        break;
      case setDistanceMode:
        gpsSettings.distance3D = !gpsSettings.distance3D;
        break;
      #endif
      case setVarioSmoothingValue:
        if (pressureSensor.smoothingValue > 0.0) {
          pressureSensor.smoothingValue = (pressureSensor.smoothingValue*100-1)/100;
        }
        break;
      case setDeadzone:
        if (pressureSensor.deadzone > 0) {
          pressureSensor.deadzone--;
        }
        break;
      #ifdef SUPPORT_MPXV7002_MPXV5004
      case setAirSpeedSensor:
        if(airSpeedSensor < MPXV7002_MPXV5004){
          airSpeedSensor++;
        }
        break;
      #endif
      #ifdef SUPPORT_TEC
      case setTECmode:
        if(TECmode < TEC_GPS){
          TECmode++;
        }
        //if(TECmode == TEC_airSpeed && airSpeedSensor == airSpeed_disabled)goto startHandleMenu;
        //if(TECmode == TEC_GPS && gpsSettings.mode == GPS_disabled)TECmode = TEC_disabled;
        break;
      #endif
      #ifdef SUPPORT_MAIN_DRIVE
      case setMainDrive:
        if (currentSensor < ACS758_200U) {
          currentSensor++;
        }
        break;
      case setCapacityMode:
        if(capacityMode < manual){
          capacityMode++;
        }
        break;
      #endif
      #ifdef SUPPORT_RX_VOLTAGE
      case enableRx1Voltage:
        enableRx1 = !enableRx1;
        break;
      case enableRx2Voltage:
        enableRx2 = !enableRx2;
        break;
      #endif
      #ifdef SUPPORT_EXT_TEMP
      case enableExternTemp:
        enableExtTemp = !enableExtTemp;
        break;
      #endif
      case saveSettings:
        #ifdef SUPPORT_GPS
        EEPROM.write(P_GPS_MODE, gpsSettings.mode);
        EEPROM.write(P_GPS_3D, gpsSettings.distance3D);
        #endif

        #ifdef SUPPORT_MAIN_DRIVE
        EEPROM.write(P_CURRENT_SENSOR, currentSensor);
        EEPROM.write(P_CAPACITY_MODE, capacityMode);
        #endif

        #ifdef SUPPORT_RX_VOLTAGE
        EEPROM.write(P_ENABLE_RX1, enableRx1);
        EEPROM.write(P_ENABLE_RX2, enableRx2);
        #endif

        #ifdef SUPPORT_EXT_TEMP
        EEPROM.write(P_ENABLE_TEMP, enableExtTemp);
        #endif

        EEPROM.write(P_VARIO_SMOOTHING,int(pressureSensor.smoothingValue*100));
        EEPROM.write(P_VARIO_DEADZONE,pressureSensor.deadzone);

        #ifdef SUPPORT_MPXV7002_MPXV5004
        EEPROM.write(P_AIRSPEED_SENSOR,airSpeedSensor);
        #endif
        
        #ifdef SUPPORT_TEC
        EEPROM.write(P_TEC_MODE,TECmode);
        #endif
    
        resetCPU();
      case defaultSettings:
        for(int i=0; i < 50; i++){
          EEPROM.write(i, 0xFF);
        }
        resetCapacity();
        resetCPU();
    }

    _bSetDisplay = true;
  }

  if ( !_bSetDisplay )
    return;

  // clear buffer
  _bufferLine1[0] = 0;
  _bufferLine2[0] = 0;

  memcpy_P( _bufferLine1, &menuText[_nMenu], 16 );

  switch ( _nMenu )
  {
    case aboutScreen:
      memcpy_P( _bufferLine2, aboutScreenText, 16 );
      break;
    #ifdef SUPPORT_GPS
    case setGpsMode:
      memcpy_P( _bufferLine2, &setGpsModeText[gpsSettings.mode], 16 );
      break;
    case setDistanceMode:
      if(gpsSettings.mode == GPS_disabled)goto startHandleMenu;
      memcpy_P( _bufferLine2, &setDistanceModeText[gpsSettings.distance3D], 16 );
      break;
    #endif
    case detectedPressureSensor:
      memcpy_P( _bufferLine2, &detectedPressureSensorText[pressureSensor.type], 16 );
      break;
    case setVarioSmoothingValue:
      if(pressureSensor.type == unknown)goto startHandleMenu;
      sprintf( _bufferLine2, " %2d%%",int(pressureSensor.smoothingValue*100));
      break;
    case setDeadzone:
      if(pressureSensor.type == unknown)goto startHandleMenu;
      sprintf( _bufferLine2, " %2dcm",pressureSensor.deadzone);
      break;
    #ifdef SUPPORT_MPXV7002_MPXV5004
    case setAirSpeedSensor:
      memcpy_P( _bufferLine2, &enableText[airSpeedSensor], 16 );
      break;
    #endif
    #ifdef SUPPORT_TEC
    case setTECmode:
      //if(airSpeedSensor == airSpeed_disabled && gpsSettings.mode == GPS_disabled)goto startHandleMenu;
      /*if(TECmode == TEC_airSpeed && airSpeedSensor == airSpeed_disabled)TECmode++;
      if(TECmode == TEC_GPS && gpsSettings.mode == GPS_disabled)TECmode++;
      if(TECmode > TEC_GPS){
        TECmode = TEC_disabled;
      }*/
      memcpy_P( _bufferLine2, &setTECmodeText[TECmode], 16 );
      break;
    #endif
    #ifdef SUPPORT_MAIN_DRIVE
    case setMainDrive:
      memcpy_P( _bufferLine2, &setMainDriveText[currentSensor], 16 );
      break;
    case setCapacityMode:
      if(currentSensor == mainDrive_disabled)goto startHandleMenu;
      memcpy_P( _bufferLine2, &setCapacityModeText[capacityMode], 16 );
      break;
    #endif
    #ifdef SUPPORT_RX_VOLTAGE
    case enableRx1Voltage:
      memcpy_P( _bufferLine2, &enableText[enableRx1], 16 );
      break;
    case enableRx2Voltage:
      memcpy_P( _bufferLine2, &enableText[enableRx2], 16 );
      break;
    #endif
    #ifdef SUPPORT_EXT_TEMP
    case enableExternTemp:
      memcpy_P( _bufferLine2, &enableText[enableExtTemp], 16 );
      break;
    #endif
  }

  jetiEx.SetJetiboxText( 0, _bufferLine1 );
  jetiEx.SetJetiboxText( 1, _bufferLine2 );

  _bSetDisplay = false;
}



