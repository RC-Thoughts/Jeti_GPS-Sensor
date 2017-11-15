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
#ifndef __BMP280_H__
#define __BMP280_H__

#if (ARDUINO >= 100)
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif

#include <Adafruit_Sensor.h>

#ifdef __AVR_ATtiny85__
 #include "TinyWireM.h"
 #define Wire TinyWireM
#else
 #include <Wire.h>
#endif

/*=========================================================================
    I2C ADDRESS/BITS
    -----------------------------------------------------------------------*/
    #define SENSOR_ADDRESS                (0x76)
/*=========================================================================*/

/*=========================================================================
    Sensor types
    -----------------------------------------------------------------------*/
    enum {
      unknown,
      BMP085_BMP180,
      BMP280,
      BME280
    };
/*=========================================================================*/

/*=========================================================================
    REGISTERS
    -----------------------------------------------------------------------*/
    enum
    {
      BMP280_REGISTER_DIG_T1              = 0x88,
      BMP280_REGISTER_DIG_T2              = 0x8A,
      BMP280_REGISTER_DIG_T3              = 0x8C,

      BMP280_REGISTER_DIG_P1              = 0x8E,
      BMP280_REGISTER_DIG_P2              = 0x90,
      BMP280_REGISTER_DIG_P3              = 0x92,
      BMP280_REGISTER_DIG_P4              = 0x94,
      BMP280_REGISTER_DIG_P5              = 0x96,
      BMP280_REGISTER_DIG_P6              = 0x98,
      BMP280_REGISTER_DIG_P7              = 0x9A,
      BMP280_REGISTER_DIG_P8              = 0x9C,
      BMP280_REGISTER_DIG_P9              = 0x9E,

      BME280_REGISTER_DIG_H1              = 0xA1,
      BME280_REGISTER_DIG_H2              = 0xE1,
      BME280_REGISTER_DIG_H3              = 0xE3,
      BME280_REGISTER_DIG_H4              = 0xE4,
      BME280_REGISTER_DIG_H5              = 0xE5,
      BME280_REGISTER_DIG_H6              = 0xE7,

      REGISTER_CHIPID                    = 0xD0,
      BMP280_REGISTER_VERSION            = 0xD1,
      BMP280_REGISTER_SOFTRESET          = 0xE0,

      BMP280_REGISTER_CAL26              = 0xE1,  // R calibration stored in 0xE1-0xF0

      BME280_REGISTER_CONTROLHUMID       = 0xF2,
      //BME280_REGISTER_STATUS             = 0XF3,
      BMP280_REGISTER_CONTROL            = 0xF4,
      BMP280_REGISTER_CONFIG             = 0xF5,
      BMP280_REGISTER_PRESSUREDATA       = 0xF7,
      BMP280_REGISTER_TEMPDATA           = 0xFA,
      BME280_REGISTER_HUMIDDATA          = 0xFD
    };

    enum
    {
      BMP085_ULTRALOWPOWER                = 0,
      BMP085_STANDARD                     = 1,
      BMP085_HIGHRES                      = 2,
      BMP085_ULTRAHIGHRES                 = 3,
      BMP085_CAL_AC1                      = 0xAA,  // R   Calibration data (16 bits)
      BMP085_CAL_AC2                      = 0xAC,  // R   Calibration data (16 bits)
      BMP085_CAL_AC3                      = 0xAE,  // R   Calibration data (16 bits)    
      BMP085_CAL_AC4                      = 0xB0,  // R   Calibration data (16 bits)
      BMP085_CAL_AC5                      = 0xB2,  // R   Calibration data (16 bits)
      BMP085_CAL_AC6                      = 0xB4,  // R   Calibration data (16 bits)
      BMP085_CAL_B1                       = 0xB6,  // R   Calibration data (16 bits)
      BMP085_CAL_B2                       = 0xB8,  // R   Calibration data (16 bits)
      BMP085_CAL_MB                       = 0xBA,  // R   Calibration data (16 bits)
      BMP085_CAL_MC                       = 0xBC,  // R   Calibration data (16 bits)
      BMP085_CAL_MD                       = 0xBE,  // R   Calibration data (16 bits)
    
      BMP085_CONTROL                      = 0xF4, 
      BMP085_TEMPDATA                     = 0xF6,
      BMP085_PRESSUREDATA                 = 0xF6,
      BMP085_READTEMPCMD                  = 0x2E,
      BMP085_READPRESSURECMD              = 0x34
    };

/*=========================================================================*/

/*=========================================================================
    CALIBRATION DATA
    -----------------------------------------------------------------------*/
    typedef struct
    {
      uint16_t dig_T1;
      int16_t  dig_T2;
      int16_t  dig_T3;

      uint16_t dig_P1;
      int16_t  dig_P2;
      int16_t  dig_P3;
      int16_t  dig_P4;
      int16_t  dig_P5;
      int16_t  dig_P6;
      int16_t  dig_P7;
      int16_t  dig_P8;
      int16_t  dig_P9;

      uint8_t  dig_H1;
      int16_t  dig_H2;
      uint8_t  dig_H3;
      int16_t  dig_H4;
      int16_t  dig_H5;
      int8_t   dig_H6;
    } bmx_calib_data;
/*=========================================================================*/


class BMx_Sensor
{
  public:
    BMx_Sensor(void);

    int  begin(uint8_t addr = SENSOR_ADDRESS);
    float readTemperature(void);
    float readPressure(void);
    float readAltitude(float seaLevelhPa = 1013.25);
    float readHumidity(void);

  private:

    void readCoefficients(void);
    int32_t computeB5(int32_t UT);
    uint16_t readRawTemperature(void);
    uint32_t readRawPressure(void);

    void      write8(byte reg, byte value);
    uint8_t   read8(byte reg);
    uint16_t  read16(byte reg);
    uint32_t  read24(byte reg);
    int16_t   readS16(byte reg);
    uint16_t  read16_LE(byte reg); // little endian
    int16_t   readS16_LE(byte reg); // little endian

    uint8_t   _i2caddr;
    uint8_t   _sensorTyp;
    int32_t   _sensorID;
    int32_t t_fine;

    //BMP085
    uint8_t oversampling;
    int16_t ac1, ac2, ac3, b1, b2, mb, mc, md;
    uint16_t ac4, ac5, ac6;

    bmx_calib_data _bmx_calib;

};

#endif
