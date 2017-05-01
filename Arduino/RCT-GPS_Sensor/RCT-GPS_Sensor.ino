/*
  -----------------------------------------------------------
                Jeti GPS Sensor v 1.0
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

GpsSensor       gps;
JetiExProtocol  jetiEx;

int units = 0;
int extended = 0;
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
int alti;

enum
{
  ID_GPSLAT       = 1,
  ID_GPSLON       = 2,
  ID_GPSSPEEDKM   = 3,
  ID_GPSSPEEDMI   = 3,
  ID_ALTME        = 4,
  ID_ALTFT        = 4,
  ID_DIST         = 5,
  ID_HEADING      = 6,
  ID_COURSE       = 7,
  ID_SATS         = 8,
  ID_HDOP         = 9,
};

JETISENSOR_CONST sensorsEU[] PROGMEM =
{
  // id             name          unit          data type           precision
  { ID_GPSLAT,      "Latitude",   " ",          JetiSensor::TYPE_GPS, 0 },
  { ID_GPSLON,      "Longitude",  " ",          JetiSensor::TYPE_GPS, 0 },
  { ID_GPSSPEEDKM,  "Speed",      "km/h",       JetiSensor::TYPE_14b, 0 },
  { ID_ALTME,       "Altitude",   "m",          JetiSensor::TYPE_14b, 0 },
  { 0 }
};

JETISENSOR_CONST sensorsEUext[] PROGMEM =
{
  // id             name          unit          data type           precision
  { ID_GPSLAT,      "Latitude",   " ",          JetiSensor::TYPE_GPS, 0 },
  { ID_GPSLON,      "Longitude",  " ",          JetiSensor::TYPE_GPS, 0 },
  { ID_GPSSPEEDKM,  "Speed",      "km/h",       JetiSensor::TYPE_14b, 0 },
  { ID_ALTME,       "Altitude",   "m",          JetiSensor::TYPE_14b, 0 },
  { ID_DIST,        "Distance",   "m",          JetiSensor::TYPE_14b, 0 },
  { ID_HEADING,     "Heading",    "\xB0",       JetiSensor::TYPE_14b, 0 },
  { ID_COURSE,      "Course",     "\xB0",       JetiSensor::TYPE_14b, 0 },
  { ID_SATS,        "Satellites", " ",          JetiSensor::TYPE_14b, 0 },
  { ID_HDOP,        "HDOP",       " ",          JetiSensor::TYPE_14b, 0 },
  { 0 }
};

JETISENSOR_CONST sensorsUS[] PROGMEM =
{
  // id             name          unit          data type           precision
  { ID_GPSLAT,      "Latitude",   " ",          JetiSensor::TYPE_GPS, 0 },
  { ID_GPSLON,      "Longitude",  " ",          JetiSensor::TYPE_GPS, 0 },
  { ID_GPSSPEEDMI,  "Speed",      "mph",        JetiSensor::TYPE_14b, 0 },
  { ID_ALTFT,       "Altitude",   "ft",         JetiSensor::TYPE_14b, 0 },
  { 0 }
};

JETISENSOR_CONST sensorsUSext[] PROGMEM =
{
  // id             name          unit          data type           precision
  { ID_GPSLAT,      "Latitude",   " ",          JetiSensor::TYPE_GPS, 0 },
  { ID_GPSLON,      "Longitude",  " ",          JetiSensor::TYPE_GPS, 0 },
  { ID_GPSSPEEDMI,  "Speed",      "mph",        JetiSensor::TYPE_14b, 0 },
  { ID_ALTFT,       "Altitude",   "ft",         JetiSensor::TYPE_14b, 0 },
  { ID_DIST,        "Distance",   "ft",         JetiSensor::TYPE_14b, 0 },
  { ID_HEADING,     "Heading",    "\xB0",       JetiSensor::TYPE_14b, 0 },
  { ID_COURSE,      "Course",     "\xB0",       JetiSensor::TYPE_14b, 0 },
  { ID_SATS,        "Satellites", " ",          JetiSensor::TYPE_14b, 0 },
  { ID_HDOP,        "HDOP",       " ",          JetiSensor::TYPE_14b, 0 },
  { 0 }
};

void setup()
{
  units = EEPROM.read(0);
  extended = EEPROM.read(1);
  if (units == 255) {
    units = 0;
  }
  if (extended == 255) {
    extended = 0;
  }
  gps.Init( GpsSensor::SERIAL3 );

  jetiEx.SetDeviceId( 0x76, 0x32 );

  if (units == 0 && extended == 0) {
    jetiEx.Start( "GPS", sensorsEU, JetiExProtocol::SERIAL2 );
  }
  if (units == 0 && extended == 1) {
    jetiEx.Start( "GPS", sensorsEUext, JetiExProtocol::SERIAL2 );
  }
  if (units == 1 && extended == 0) {
    jetiEx.Start( "GPS", sensorsUS, JetiExProtocol::SERIAL2 );
  }
  if (units == 1 && extended == 1) {
    jetiEx.Start( "GPS", sensorsUSext, JetiExProtocol::SERIAL2 );
  }
}

void loop()
{
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
    if (units == 0) {
      jetiEx.SetSensorValueGPS( ID_GPSLAT, false, lat );
      jetiEx.SetSensorValueGPS( ID_GPSLON, true, lng );
      jetiEx.SetSensorValue( ID_ALTME, gps.GetAltMe() );
      jetiEx.SetSensorValue( ID_GPSSPEEDKM, gps.GetSpeedKm() );
    }
    if (units == 1) {
      jetiEx.SetSensorValueGPS( ID_GPSLAT, false, lat );
      jetiEx.SetSensorValueGPS( ID_GPSLON, true, lng );
      jetiEx.SetSensorValue( ID_ALTFT, gps.GetAltFt() );
      jetiEx.SetSensorValue( ID_GPSSPEEDMI, gps.GetSpeedMi() );
    }
    if (extended == 1) {
      jetiEx.SetSensorValue( ID_HEADING, gps.GetCourseDeg() );
      jetiEx.SetSensorValue( ID_SATS, gps.GetSats() );
      jetiEx.SetSensorValue( ID_HDOP, gps.GetHDOP() );
    }

    if (!homeSet && homeTime > 0 && (homeTime < millis())) {
      homeSet = true;
      home_lat = lat;
      home_lon = lng;
      if (units == 0) {
        home_alt = gps.GetAltMe();
      }
      if (units == 1) {
        home_alt = gps.GetAltFt();
      }
    }

    if (homeSet) {
      if (extended == 1) {
        // Distance to Model
        distToHome =
          TinyGPSPlus::distanceBetween(
            gps.GetLat(),
            gps.GetLon(),
            home_lat,
            home_lon);
        if (units == 1) {
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
      if (units == 0) {
        if (home_alt >= 0) {
          alti = (gps.GetAltMe() - home_alt);
          if (alti < 0) {
            alti = 0;
          }
          jetiEx.SetSensorValue( ID_ALTME, alti );
        } else {
          alti = (gps.GetAltMe() + home_alt);
          if (alti < 0) {
            alti = 0;
          }
          jetiEx.SetSensorValue( ID_ALTME, alti );
        }
      }
      if (units == 1) {
        if (home_alt >= 0) {
          alti = (gps.GetAltFt() - home_alt);
          if (alti < 0) {
            alti = 0;
          }
          jetiEx.SetSensorValue( ID_ALTME, alti );
        } else {
          alti = (gps.GetAltFt() + home_alt);
          if (alti < 0) {
            alti = 0;
          }
          jetiEx.SetSensorValue( ID_ALTME, alti );
        }
      }
    } else {
      distToHome = 0;
      courseToModel = 0;
    }
    if (extended == 1) {
      jetiEx.SetSensorValue( ID_DIST, distToHome );
      jetiEx.SetSensorValue( ID_COURSE, courseToModel );
    }
  } else {
    lat = 0;
    lng = 0;
    if (units == 0) {
      jetiEx.SetSensorValueGPS( ID_GPSLAT, false, lat );
      jetiEx.SetSensorValueGPS( ID_GPSLON, true, lng );
      jetiEx.SetSensorValue( ID_ALTME, 0 );
      jetiEx.SetSensorValue( ID_GPSSPEEDKM, 0 );
    }
    if (units == 1) {
      jetiEx.SetSensorValueGPS( ID_GPSLAT, false, lat );
      jetiEx.SetSensorValueGPS( ID_GPSLON, true, lng );
      jetiEx.SetSensorValue( ID_ALTFT, 0 );
      jetiEx.SetSensorValue( ID_GPSSPEEDMI, 0 );
    }
    if (extended == 1) {
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
  if ( c == 0xe0 && _nMenu < 5 )
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
      units = 0;
      EEPROM.write(0, units);
      _nMenu = 6;
      _bSetDisplay = true;
    }
    if ( _nMenu == 2 ) {
      units = 1;
      EEPROM.write(0, units);
      _nMenu = 6;
      _bSetDisplay = true;
    }
    if ( _nMenu == 3 ) {
      extended = 0;
      EEPROM.write(1, extended);
      _nMenu = 6;
      _bSetDisplay = true;
    }
    if ( _nMenu == 4 ) {
      extended = 1;
      EEPROM.write(1, extended);
      _nMenu = 6;
      _bSetDisplay = true;
    }
    if ( _nMenu == 5 ) {
      units = 0;
      extended = 0;
      EEPROM.write(0, units);
      EEPROM.write(1, extended);
      _nMenu = 6;
      _bSetDisplay = true;
    }
  }

  if ( !_bSetDisplay )
    return;

  switch ( _nMenu )
  {
    case 0:
      jetiEx.SetJetiboxText( JetiExProtocol::LINE1, " RCT Jeti Tools" );
      jetiEx.SetJetiboxText( JetiExProtocol::LINE2, "   GPS Sensor" );
      _bSetDisplay = false;
      break;
    case 1:
      jetiEx.SetJetiboxText( JetiExProtocol::LINE1, "  Meter & km/h" );
      jetiEx.SetJetiboxText( JetiExProtocol::LINE2, "  Store: DOWN" );
      _bSetDisplay = false;
      break;
    case 2:
      jetiEx.SetJetiboxText( JetiExProtocol::LINE1, "  Feet & mph" );
      jetiEx.SetJetiboxText( JetiExProtocol::LINE2, "  Store: DOWN" );
      _bSetDisplay = false;
      break;
    case 3:
      jetiEx.SetJetiboxText( JetiExProtocol::LINE1, "  Basic mode" );
      jetiEx.SetJetiboxText( JetiExProtocol::LINE2, "  Store: DOWN" );
      _bSetDisplay = false;
      break;
    case 4:
      jetiEx.SetJetiboxText( JetiExProtocol::LINE1, " Extended mode" );
      jetiEx.SetJetiboxText( JetiExProtocol::LINE2, "  Store: DOWN" );
      _bSetDisplay = false;
      break;
    case 5:
      jetiEx.SetJetiboxText( JetiExProtocol::LINE1, " Reset defaults" );
      jetiEx.SetJetiboxText( JetiExProtocol::LINE2, "  Store: DOWN" );
      _bSetDisplay = false;
      break;
    case 6:
      jetiEx.SetJetiboxText( JetiExProtocol::LINE1, "Settings stored!" );
      jetiEx.SetJetiboxText( JetiExProtocol::LINE2, " Search sensors!" );
      _bSetDisplay = false;
      break;
      if (_nMenu == 6) {
        delay(1500);
        _nMenu = 0;
        _bSetDisplay = true;
      }
  }
}
