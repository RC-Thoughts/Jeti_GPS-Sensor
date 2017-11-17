/***************************************************************************
  This is a library for the BMP280 pressure sensor

  Designed specifically to work with the Adafruit BMP280 Breakout
  ----> http://www.adafruit.com/products/2651

  These sensors use I2C to communicate, 2 pins are required to interface.

  Adafruit invests time and resources providing this open source code,
  please support Adafruit andopen-source hardware by purchasing products
  from Adafruit!

  Written by Kevin Townsend for Adafruit Industries.
  BSD license, all text above must be included in any redistribution
 ***************************************************************************/
#include "Arduino.h"
#include <Wire.h>
#include "BMx_Sensor.h"


/***************************************************************************
 PRIVATE FUNCTIONS
 ***************************************************************************/

BMx_Sensor::BMx_Sensor(){ }

//bool BMx_Sensor::begin(uint8_t a) {
int BMx_Sensor::begin(uint8_t a) {
  _i2caddr = a;

  // i2c
  Wire.begin();

  switch (read8(REGISTER_CHIPID))
    {
      case 0x58:
        _sensorTyp = BMP280;
        BMx280_Init();
        break;      
      case 0x60:
        _sensorTyp = BME280;
        BMx280_Init();
        break;
      default:
        _sensorTyp = unknown;
        break;
    }
  
  return _sensorTyp;
}

void BMx_Sensor::BMx280_Init(){

  // reset the device using soft-reset
  // this makes sure the IIR is off, etc.
  write8(BMP280_REGISTER_SOFTRESET, 0xB6);

  // wait for chip to wake up.
  delay(100);
    
  readCoefficients(); // read trimming parameters

  setSampling(); // use defaults
}

/**************************************************************************/
/*!
    @brief  setup sensor with given parameters / settings
    
    This is simply a overload to the normal begin()-function, so SPI users
    don't get confused about the library requiring an address.
*/
/**************************************************************************/


void BMx_Sensor::setSampling(sensor_mode       mode,
                             sensor_sampling   tempSampling,
                             sensor_sampling   pressSampling,
                             sensor_sampling   humSampling,
                             sensor_filter     filter,
                             standby_duration  duration) 
                             {
   
    _measReg.mode     = mode;
    _measReg.osrs_t   = tempSampling;
    _measReg.osrs_p   = pressSampling;
        
    
    _humReg.osrs_h    = humSampling;
    _configReg.filter = filter;
    _configReg.t_sb   = duration;
    
    // you must make sure to also set REGISTER_CONTROL after setting the
    // CONTROLHUMID register, otherwise the values won't be applied (see DS 5.4.3)
    if(_sensorTyp == BME280)write8(BME280_REGISTER_CONTROLHUMID, _humReg.get());
    write8(BMP280_REGISTER_CONFIG, _configReg.get());
    write8(BMP280_REGISTER_CONTROL, _measReg.get());
}


/**************************************************************************/
/*!
    @brief  Writes an 8 bit value over I2C/SPI
*/
/**************************************************************************/
void BMx_Sensor::write8(byte reg, byte value)
{
  Wire.beginTransmission((uint8_t)_i2caddr);
  Wire.write((uint8_t)reg);
  Wire.write((uint8_t)value);
  Wire.endTransmission();
}

/**************************************************************************/
/*!
    @brief  Reads an 8 bit value over I2C
*/
/**************************************************************************/
uint8_t BMx_Sensor::read8(byte reg)
{
  uint8_t value;

  Wire.beginTransmission((uint8_t)_i2caddr);
  Wire.write((uint8_t)reg);
  Wire.endTransmission();
  Wire.requestFrom((uint8_t)_i2caddr, (byte)1);
  value = Wire.read();

  return value;
}

/**************************************************************************/
/*!
    @brief  Reads a 16 bit value over I2C
*/
/**************************************************************************/
uint16_t BMx_Sensor::read16(byte reg)
{
  uint16_t value;

  Wire.beginTransmission((uint8_t)_i2caddr);
  Wire.write((uint8_t)reg);
  Wire.endTransmission();
  Wire.requestFrom((uint8_t)_i2caddr, (byte)2);
  value = (Wire.read() << 8) | Wire.read();

  return value;
}

uint16_t BMx_Sensor::read16_LE(byte reg) {
  uint16_t temp = read16(reg);
  return (temp >> 8) | (temp << 8);

}

/**************************************************************************/
/*!
    @brief  Reads a signed 16 bit value over I2C
*/
/**************************************************************************/
int16_t BMx_Sensor::readS16(byte reg)
{
  return (int16_t)read16(reg);

}

int16_t BMx_Sensor::readS16_LE(byte reg)
{
  return (int16_t)read16_LE(reg);

}


/**************************************************************************/
/*!
    @brief  Reads a signed 16 bit value over I2C
*/
/**************************************************************************/

uint32_t BMx_Sensor::read24(byte reg)
{
  uint32_t value;

  Wire.beginTransmission((uint8_t)_i2caddr);
  Wire.write((uint8_t)reg);
  Wire.endTransmission();
  Wire.requestFrom((uint8_t)_i2caddr, (byte)3);
  
  value = Wire.read();
  value <<= 8;
  value |= Wire.read();
  value <<= 8;
  value |= Wire.read();

  return value;
}

/**************************************************************************/
/*!
    @brief  Reads the factory-set coefficients
*/
/**************************************************************************/
void BMx_Sensor::readCoefficients(void)
{
    _bmx_calib.dig_T1 = read16_LE(BMP280_REGISTER_DIG_T1);
    _bmx_calib.dig_T2 = readS16_LE(BMP280_REGISTER_DIG_T2);
    _bmx_calib.dig_T3 = readS16_LE(BMP280_REGISTER_DIG_T3);

    _bmx_calib.dig_P1 = read16_LE(BMP280_REGISTER_DIG_P1);
    _bmx_calib.dig_P2 = readS16_LE(BMP280_REGISTER_DIG_P2);
    _bmx_calib.dig_P3 = readS16_LE(BMP280_REGISTER_DIG_P3);
    _bmx_calib.dig_P4 = readS16_LE(BMP280_REGISTER_DIG_P4);
    _bmx_calib.dig_P5 = readS16_LE(BMP280_REGISTER_DIG_P5);
    _bmx_calib.dig_P6 = readS16_LE(BMP280_REGISTER_DIG_P6);
    _bmx_calib.dig_P7 = readS16_LE(BMP280_REGISTER_DIG_P7);
    _bmx_calib.dig_P8 = readS16_LE(BMP280_REGISTER_DIG_P8);
    _bmx_calib.dig_P9 = readS16_LE(BMP280_REGISTER_DIG_P9);

    if(_sensorTyp == BME280){
      _bmx_calib.dig_H1 = read8(BME280_REGISTER_DIG_H1);
      _bmx_calib.dig_H2 = readS16_LE(BME280_REGISTER_DIG_H2);
      _bmx_calib.dig_H3 = read8(BME280_REGISTER_DIG_H3);
      _bmx_calib.dig_H4 = (read8(BME280_REGISTER_DIG_H4) << 4) | (read8(BME280_REGISTER_DIG_H4+1) & 0xF);
      _bmx_calib.dig_H5 = (read8(BME280_REGISTER_DIG_H5+1) << 4) | (read8(BME280_REGISTER_DIG_H5) >> 4);
      _bmx_calib.dig_H6 = (int8_t)read8(BME280_REGISTER_DIG_H6);
    }
}

/**************************************************************************/
/*!

*/
/**************************************************************************/
float BMx_Sensor::readTemperature(void)
{
   
  int32_t var1, var2;

  int32_t adc_T = read24(BMP280_REGISTER_TEMPDATA);
  adc_T >>= 4;

  var1  = ((((adc_T>>3) - ((int32_t)_bmx_calib.dig_T1 <<1))) *
	   ((int32_t)_bmx_calib.dig_T2)) >> 11;

  var2  = (((((adc_T>>4) - ((int32_t)_bmx_calib.dig_T1)) *
	     ((adc_T>>4) - ((int32_t)_bmx_calib.dig_T1))) >> 12) *
	   ((int32_t)_bmx_calib.dig_T3)) >> 14;

  t_fine = var1 + var2;

  float T  = (t_fine * 5 + 128) >> 8;
  return T/100;
}

/**************************************************************************/
/*!

*/
/**************************************************************************/
float BMx_Sensor::readPressure(void) {
  
  int64_t var1, var2, p;

  // Must be done first to get the t_fine variable set up
  readTemperature();

  int32_t adc_P = read24(BMP280_REGISTER_PRESSUREDATA);
  adc_P >>= 4;

  var1 = ((int64_t)t_fine) - 128000;
  var2 = var1 * var1 * (int64_t)_bmx_calib.dig_P6;
  var2 = var2 + ((var1*(int64_t)_bmx_calib.dig_P5)<<17);
  var2 = var2 + (((int64_t)_bmx_calib.dig_P4)<<35);
  var1 = ((var1 * var1 * (int64_t)_bmx_calib.dig_P3)>>8) +
    ((var1 * (int64_t)_bmx_calib.dig_P2)<<12);
  var1 = (((((int64_t)1)<<47)+var1))*((int64_t)_bmx_calib.dig_P1)>>33;

  if (var1 == 0) {
    return 0;  // avoid exception caused by division by zero
  }
  p = 1048576 - adc_P;
  p = (((p<<31) - var2)*3125) / var1;
  var1 = (((int64_t)_bmx_calib.dig_P9) * (p>>13) * (p>>13)) >> 25;
  var2 = (((int64_t)_bmx_calib.dig_P8) * p) >> 19;

  p = ((p + var1 + var2) >> 8) + (((int64_t)_bmx_calib.dig_P7)<<4);
  return (float)p/256;
}


/**************************************************************************/
/*!
    @brief  Returns the humidity from the sensor
*/
/**************************************************************************/
float BMx_Sensor::readHumidity(void) {
    readTemperature(); // must be done first to get t_fine

    int32_t adc_H = read16(BME280_REGISTER_HUMIDDATA);
    if (adc_H == 0x8000) // value in case humidity measurement was disabled
        return NAN;
        
    int32_t v_x1_u32r;

    v_x1_u32r = (t_fine - ((int32_t)76800));

    v_x1_u32r = (((((adc_H << 14) - (((int32_t)_bmx_calib.dig_H4) << 20) -
                    (((int32_t)_bmx_calib.dig_H5) * v_x1_u32r)) + ((int32_t)16384)) >> 15) *
                 (((((((v_x1_u32r * ((int32_t)_bmx_calib.dig_H6)) >> 10) *
                      (((v_x1_u32r * ((int32_t)_bmx_calib.dig_H3)) >> 11) + ((int32_t)32768))) >> 10) +
                    ((int32_t)2097152)) * ((int32_t)_bmx_calib.dig_H2) + 8192) >> 14));

    v_x1_u32r = (v_x1_u32r - (((((v_x1_u32r >> 15) * (v_x1_u32r >> 15)) >> 7) *
                               ((int32_t)_bmx_calib.dig_H1)) >> 4));

    v_x1_u32r = (v_x1_u32r < 0) ? 0 : v_x1_u32r;
    v_x1_u32r = (v_x1_u32r > 419430400) ? 419430400 : v_x1_u32r;
    float h = (v_x1_u32r>>12);
    return h / 1024.0;
}

/**************************************************************************/
/*!
    Calculates the altitude (in meters) from the specified atmospheric
    pressure (in hPa), and sea-level pressure (in hPa).

    @param  seaLevel      Sea-level pressure in hPa
*/
/**************************************************************************/
float BMx_Sensor::readAltitude(float seaLevelhPa) {
  float altitude;

  float pressure = readPressure(); // in Si units for Pascal
  pressure /= 100;

  altitude = 44330 * (1.0 - pow(pressure / seaLevelhPa, 0.1903));

  return altitude;
}
