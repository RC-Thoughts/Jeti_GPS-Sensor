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
  setFilterX,
  setFilterY,
  setDeadzone,
  setMainDrive,
  setCapacityMode,
  enableRx1Voltage,
  enableRx2Voltage,
  enableExternTemp,
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
  {"Vario filter"},
  {"Vario filter"},
  {"Vario deadzone:"},
  {"Main drive:"},
  {"Capacity reset:"},
  {"Rx1 voltage:"},
  {"Rx2 voltage:"},
  {"Ext. Temp:"},
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
      case setFilterX:
        if (pressureSensor.filterX < 0.98) {
          pressureSensor.filterX += 0.01;
        }
        break;
      case setFilterY:
        if (pressureSensor.filterY < 0.98) {
          pressureSensor.filterY += 0.01;
        }
        break;
      case setDeadzone:
        if (pressureSensor.deadzone < 100) {
          pressureSensor.deadzone++;
        }
        break;
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
    }
    
    _bSetDisplay = true;
  }
  
  // DN
  if ( c == keyDown )
  {
    switch ( _nMenu )
    {
      case resetOffset:
        EEPROM.put(EEPROM_ADRESS_CAPACITY, 0.0f);                 // reset capacity in eeprom
        EEPROM.put(EEPROM_ADRESS_CAPACITY+sizeof(float), 0.0f);
        resetFunc();
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
      case setFilterX:
        if (pressureSensor.filterX > 0.01) {
          pressureSensor.filterX -= 0.01;
        }
        break;
      case setFilterY:
        if (pressureSensor.filterY > 0.01) {
          pressureSensor.filterY -= 0.01;
        }
        break;
      case setDeadzone:
        if (pressureSensor.deadzone > 0) {
          pressureSensor.deadzone--;
        }
        break;
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
      case enableRx1Voltage:
        enableRx1 = !enableRx1;
        break;
      case enableRx2Voltage:
        enableRx2 = !enableRx2;
        break;
      case enableExternTemp:
        enableExtTemp = !enableExtTemp;
        break;
      case saveSettings:
        EEPROM.write(1, gpsSettings.mode);
        EEPROM.write(2, gpsSettings.distance3D);
        EEPROM.write(3, currentSensor);
        EEPROM.write(5, capacityMode);
        EEPROM.write(6, enableRx1);
        EEPROM.write(7, enableRx2);
        EEPROM.write(8, enableExtTemp);
        EEPROM.write(10,int(pressureSensor.filterX*100));
        EEPROM.write(11,int(pressureSensor.filterY*100));
        EEPROM.write(12,pressureSensor.deadzone);
        resetFunc();
      case defaultSettings:
        for(int i=0; i < 50; i++){
          EEPROM.write(i, 0xFF);
        }
        EEPROM.put(EEPROM_ADRESS_CAPACITY, 0.0f);
        EEPROM.put(EEPROM_ADRESS_CAPACITY+sizeof(float), 0.0f);
        resetFunc();
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
    case setFilterX:
      if(pressureSensor.type == unknown)goto startHandleMenu;
      sprintf( _bufferLine2, " X: 0.%2d",int(pressureSensor.filterX*100));
      break;
    case setFilterY:
      if(pressureSensor.type == unknown)goto startHandleMenu;
      sprintf( _bufferLine2, " Y: 0.%2d",int(pressureSensor.filterY*100));
      break;
    case setDeadzone:
      if(pressureSensor.type == unknown)goto startHandleMenu;
      sprintf( _bufferLine2, " %2dcm",pressureSensor.deadzone);
      break;
    case setMainDrive:
      memcpy_P( _bufferLine2, &setMainDriveText[currentSensor], 16 );
      break;
    case setCapacityMode:
      if(currentSensor == mainDrive_disabled)goto startHandleMenu;
      memcpy_P( _bufferLine2, &setCapacityModeText[capacityMode], 16 );
      break;
    case enableRx1Voltage:
      memcpy_P( _bufferLine2, &enableText[enableRx1], 16 );
      break;
    case enableRx2Voltage:
      memcpy_P( _bufferLine2, &enableText[enableRx2], 16 );
      break;
    case enableExternTemp:
      memcpy_P( _bufferLine2, &enableText[enableExtTemp], 16 );
      break;
  }
  
  jetiEx.SetJetiboxText( JetiExProtocol::LINE1, _bufferLine1 );
  jetiEx.SetJetiboxText( JetiExProtocol::LINE2, _bufferLine2 );
  
  _bSetDisplay = false;
}
