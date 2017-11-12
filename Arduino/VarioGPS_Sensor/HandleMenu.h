/*
  -----------------------------------------------------------
            Menu handler for Jetibox
  -----------------------------------------------------------
*/


// Jetibox screens, names and order
enum screenViews {
  aboutScreen,
  resetAltitude,
  setGpsMode,
  setDistanceMode,
  setVolt1,
  setVolt2,
  setUnits,
  saveSettings,
  defaultSettings
};


void HandleMenu()
{ 
  static int  _nMenu = aboutScreen;
  static bool _bSetDisplay = true;
  uint8_t c = jetiEx.GetJetiboxKey();

  // 224 0xe0 : // RIGHT
  // 112 0x70 : // LEFT
  // 208 0xd0 : // UP
  // 176 0xb0 : // DOWN
  // 144 0x90 : // UP+DOWN
  //  96 0x60 : // LEFT+RIGHT

  // Right
  if ( c == 0xe0 && _nMenu < defaultSettings)
  {
    _nMenu++;
    _bSetDisplay = true;
  }

  // Left
  if ( c == 0x70 &&  _nMenu > aboutScreen )
  {
    _nMenu--;
    _bSetDisplay = true;
  }

  // DN
  if ( c == 0xb0 )
  {
    switch ( _nMenu )
    {
      case resetAltitude:
        resetFunc();
      case setUnits:
        if(units == EU){
          units = US;
        }else{
          units = EU;
        }
        break;
      case setGpsMode:
        if(gpsSettings.mode == disabled){
          gpsSettings.mode = basic;
        }else if(gpsSettings.mode == basic){
          gpsSettings.mode = extended;
        }else if(gpsSettings.mode == extended){
          gpsSettings.mode = disabled;
        }
        break;
      case setDistanceMode:
        gpsSettings.distance3D = !gpsSettings.distance3D;
        break;
      case setVolt1:
        volt1Enable = !volt1Enable;
        break;
      case setVolt2:
        volt2Enable = !volt2Enable;
        break;
      case saveSettings:
        EEPROM.write(0, units);
        EEPROM.write(1, gpsSettings.mode);
        EEPROM.write(2, gpsSettings.distance3D);
        EEPROM.write(3, volt1Enable);
        EEPROM.write(4, volt2Enable);
        resetFunc();
      case defaultSettings:
        for(int i=0; i < 10; i++){
          EEPROM.write(i, 0xFF);
        }
        resetFunc();
    }
    _bSetDisplay = true;
  }

  if ( !_bSetDisplay )
    return;

  switch ( _nMenu )
  {
    case aboutScreen:
      jetiEx.SetJetiboxText( JetiExProtocol::LINE1, "VarioGPS Sensor" );
      jetiEx.SetJetiboxText( JetiExProtocol::LINE2, VARIOGPS_VERSION );
      break;
    case resetAltitude:
      jetiEx.SetJetiboxText( JetiExProtocol::LINE1, "Altitude Reset?" );
      jetiEx.SetJetiboxText( JetiExProtocol::LINE2, "Press: Down" );
      break;
    case setUnits:
      if(units == EU){
        jetiEx.SetJetiboxText( JetiExProtocol::LINE1, "Units: \x45\x55\x28\x6D\x2C\xDF\x43\x29" );
      }else{
        jetiEx.SetJetiboxText( JetiExProtocol::LINE1, "Units: \x55\x53\x28\x66\x74\x2C\xDF\x46\x29" );
      }
      jetiEx.SetJetiboxText( JetiExProtocol::LINE2, "Change: Down" );
      break;
    case setGpsMode:
      if(gpsSettings.mode == disabled){
        jetiEx.SetJetiboxText( JetiExProtocol::LINE1, "GPS: Disabled" );
      }else if(gpsSettings.mode == basic){
        jetiEx.SetJetiboxText( JetiExProtocol::LINE1, "GPS: Basic" );
      }else if(gpsSettings.mode == extended){
        jetiEx.SetJetiboxText( JetiExProtocol::LINE1, "GPS: Extended" );
      }  
      jetiEx.SetJetiboxText( JetiExProtocol::LINE2, "Change: Down" );
      break;
    case setDistanceMode:
      if(gpsSettings.distance3D){
        jetiEx.SetJetiboxText( JetiExProtocol::LINE1, "GPS distance: 3D" );
      }else{
        jetiEx.SetJetiboxText( JetiExProtocol::LINE1, "GPS distance: 2D" );
      }
      jetiEx.SetJetiboxText( JetiExProtocol::LINE2, "Change: Down" );
      break;
    case setVolt1:
      if(volt1Enable){
        jetiEx.SetJetiboxText( JetiExProtocol::LINE1, "Volt1: Enabled" );
      }else{
        jetiEx.SetJetiboxText( JetiExProtocol::LINE1, "Volt1: Disabled" );
      }
      jetiEx.SetJetiboxText( JetiExProtocol::LINE2, "Change: Down" );
      break;
    case setVolt2:
      if(volt2Enable){
        jetiEx.SetJetiboxText( JetiExProtocol::LINE1, "Volt2: Enabled" );
      }else{
        jetiEx.SetJetiboxText( JetiExProtocol::LINE1, "Volt2: Disabled" );
      }
      jetiEx.SetJetiboxText( JetiExProtocol::LINE2, "Change: Down" );
      break;
    case saveSettings:
      jetiEx.SetJetiboxText( JetiExProtocol::LINE1, "Save and Restart" );
      jetiEx.SetJetiboxText( JetiExProtocol::LINE2, "Save: Down" );
      break;
    case defaultSettings:
      jetiEx.SetJetiboxText( JetiExProtocol::LINE1, "Reset Defaults" );
      jetiEx.SetJetiboxText( JetiExProtocol::LINE2, "Reset: Down" );
      break;
  }
  _bSetDisplay = false;
}
