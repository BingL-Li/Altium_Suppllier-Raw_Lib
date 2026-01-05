/**
 * \brief This header file contains all register map definitions for the ADS131M0x device family.
 *
 * \copyright Copyright (C) 2019 Texas Instruments Incorporated - http://www.ti.com/
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *    Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 *    Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the
 *    distribution.
 *
 *    Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#ifndef TMAG5x73_H_
#define TMAG5x73_H_

// Standard libraries
#include <assert.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>

// Custom libraries
#include "hal.h"


//****************************************************************************
//
// Constants
//
//****************************************************************************

#define NUM_REGISTERS                           ((uint8_t) 29)

// Length of an I2C frame for the TMAG5x73 in bytes
#define TMAG5x73_FRAME_NUM_BYTES                    1

//****************************************************************************
//****************************************************************************
//
// Function prototypes
//
//****************************************************************************
//****************************************************************************

void TMAG5x73startup();
void TMAG5x73restoreDefaultValues();
void TMAG5x73updateI2Caddress (uint8_t i2c_address);

//***************//
// I2C Functions //
//***************//
void    TMAG5x73writeToSingleRegister(uint8_t address, uint8_t data);
void    TMAG5x73writeToRegisterWithTrigger(uint8_t address, uint8_t data);
void    TMAG5x73writeToMultipleRegisters(uint8_t startAddress, const uint8_t count, uint8_t triggerBit, const uint8_t data[]);
uint8_t TMAG5x73readSingleRegister(uint8_t address, uint8_t triggerBit);
void    TMAG5x73readMultipleRegisters(uint8_t startAddress, uint8_t count, uint8_t triggerBit, uint8_t data_read_results[]);
void    TMAG5x73oneByteRead(uint8_t numChannels, uint8_t bitRead, uint8_t crc_en, uint8_t data_read_results[]);
void    TMAG5x73setI2CreadMode(uint8_t read_mode);
void    TMAG5x73enableCRC();
void    TMAG5x73disableCRC();
void    TMAG5x73i2cGlitchFilterEnabled();
void    TMAG5x73i2cGlitchFilterDisabled();

//******************************//
// Change Device Operation Mode //
//******************************//
void TMAG5x73enterStandbyMode();
void TMAG5x73enterContinuousMeasureMode();
void TMAG5x73enterSleepMode();
void TMAG5x73enterWakeUpAndSleepMode();
void TMAG5x73setSLEEPTIME(uint8_t sleeptime);
void TMAG5x73setWakeUpAndSleepMode(uint8_t sleeptime);
void TMAG5x73exitSleepMode();
void TMAG5x73exitWakeAndSleepMode();

//*****************************************//
// Functions to Configure Trigger Settings //
//*****************************************//
void TMAG5x73i2cTriggersConversion();
void TMAG5x73intTriggersConversion(uint8_t i2c_busy);

//*******************************************//
// Threshold Detection + INT output Settings //
//*******************************************//
void TMAG5x73intPinLatchedPulsed(uint8_t i2c_busy, uint8_t int_state);
void TMAG5x73intIndicatesConversionEnable(uint8_t i2c_busy);
void TMAG5x73sclIndicatesConversionEnable(uint8_t i2c_busy);
void TMAG5x73interruptIndicatesConversionDisable();
void TMAG5173setMagSwitch(uint8_t switch_type);
void TMAG5x73disableInterruptModes();
void TMAG5x73disableMaskINTpin();
void TMAG5x73thresholdSettings(uint8_t thr_direction, uint8_t thr_crossings, uint8_t thr_hyst);
void TMAG5x73magThreshSet(uint8_t set_threshold, int8_t thr);
void TMAG5x73disableMagThresholds();

//*************************************//
// Measurement Configuration Functions //
//*************************************//
void TMAG5x73enableMagChannels(uint8_t mag_ch_en_bits);
void TMAG5x73enableAngleMeasurement(uint8_t angle_en_bits);
void TMAG5x73enableTemperatureMeasurement();
void TMAG5x73disableTemperatureMeasurement();
void TMAG5x73setTempCoefficient(uint8_t temp_coefficient);
void TMAG5x73setSampleRate(uint8_t CONV_AVG_bits);
void TMAG5x73setRanges(uint8_t x_y_range_bits, uint8_t z_range_bits);
void TMAG5x73selectLowCurrentNoiseMode(uint8_t low_mode);

//*******************************************************************//
// Get Results/Measurement Functions (Standard 3-byte I2C Read Only) //
//*******************************************************************//
uint8_t TMAG5x73getXMSBresult();
uint8_t TMAG5x73getXLSBresult();
uint8_t TMAG5x73getYMSBresult();
uint8_t TMAG5x73getYLSBresult();
uint8_t TMAG5x73getZMSBresult();
uint8_t TMAG5x73getZLSBresult();
uint8_t TMAG5x73getTempMSBresult();
uint8_t TMAG5x73getTempLSBresult();
uint8_t TMAG5x73getAngleMSBresult();
uint8_t TMAG5x73getAngleLSBresult();
uint8_t TMAG5x73getMAGresult();
void    TMAG5x73getMagResultsRegisters(int16_t meas_arr[]);
float   TMAG5x73getMeasurementX();
float   TMAG5x73getMeasurementY();
float   TMAG5x73getMeasurementZ();
float   TMAG5x73getMeasurementTEMP();
float   TMAG5x73getMeasurementANGLE();
float   TMAG5x73getMeasurementMAG();
void    TMAG5x73getMagMeasurements(float meas_arr[]);

//***********************************************************//
// Get Results/Measurement Functions (1-byte I2C Reads Only) //
//***********************************************************//
void TMAG5x73getOneByteMeasurements(const uint8_t dataChannels, const uint8_t bitRead, const uint16_t xy_range, const uint16_t z_range, float data[]);

//*********************//
// Get Range Functions //
//*********************//
uint16_t TMAG5x73getXYrange();
uint16_t TMAG5x73getZrange();

//***************************//
// Get Device Info Functions //
//***************************//
uint8_t TMAG5x73getVersion();
uint8_t TMAG5x73isCRCenabled();

//**************************************//
// Offset and Gain Correction Functions //
//**************************************//
void TMAG5x73setMagOffsetIn8Bit(uint8_t offset1_bits, uint8_t offset2_bits);
void TMAG5x73setMagOffsetInmT(const uint8_t offset_select, const float offset1_delta , const float offset2_delta);
void TMAG5x73setMagGainConfigIn8Bit(uint8_t channel, uint16_t gain_bits);
void TMAG5x73setMagGainConfigInDecimal(uint8_t channel, float gain_value);

//************************//
// Supplemental Functions //
//************************//
void     calcCORDIC(float CORDIC_results[], int16_t numerator, int16_t denominator, uint16_t range, int16_t iteration_length);
void     atan2CORDIC(int16_t numerator, int16_t denominator, int16_t iteration_length, int32_t results[]);
void     planeAngles(int16_t axisX, int16_t axisY, int16_t axisZ,  int32_t results[]);
void     convertToSpherical(int16_t axis1, int16_t axis2, int16_t axis3,  int32_t results[]);
void     convertToCylindrical(int16_t axis1, int16_t axis2, int16_t axis3,  int32_t results[]);
uint32_t mag3D(int16_t axis1, int16_t axis2, int16_t axis3);
float    piecewiseLinearizationRegister(int16_t knownValue[], float knownError[], uint16_t known_length, int16_t measValue, uint16_t range);
float    piecewiseLinearizationFloat(float knownValue[], float knownError[], uint16_t known_length, float measValue);
float    piecewiseLinearizationAngle(uint16_t knownAngle[], float knownError[], uint16_t known_length, uint16_t measAngle);
float    piecewiseLinearizationAngleFloat(float knownAngle[], float knownError[], uint16_t known_length, float measAngle);

//******************//
// Helper Functions //
//******************//
uint32_t isqrt32(uint32_t h);
float    TMAG5x73resultRegisterTomT(int16_t register_bits, uint16_t range);
float    TMAG5x73angleRegisterToDeg(uint16_t register_bits);
uint8_t  TMAG5x73calculateCRC(uint8_t i2cRead, uint8_t numChannels, uint8_t data[]);
uint8_t  TMAG5x73verifyCRC(uint8_t i2cRead, uint8_t numChannels, uint8_t data[]);
void     TMAG5x73intPulse();

//****************************************************************************
//
// Definitions for use with Functions
//
//****************************************************************************

#define SENSOR_CONFIG_2_FULL_RANGE_MASK                 ((uint8_t) 0x03)

// These 'BITS' definitions are for use as function inputs

#define Trigger_Conversion_Disabled                     ((uint8_t) 0x00)
#define Trigger_Conversion_Enabled                      ((uint8_t) 0x01)

#define I2C_Busy_Interrupt                              ((uint8_t) 0x00)
#define I2C_Busy_Wait                                   ((uint8_t) 0x01)

#define INT_STATE_Latched                               ((uint8_t) 0x00)
#define INT_STATE_Pulse                                 ((uint8_t) 0x01)

#define Unipolar_Switch_Mode                            ((uint8_t) 0x00)    /* TMAG5173 Only */
#define Omnipolar_Switch_Mode                           ((uint8_t) 0x01)    /* TMAG5173 Only */

#define MAG_CH_EN_Off                                   ((uint8_t) 0x00)
#define MAG_CH_EN_X                                     ((uint8_t) 0x01)
#define MAG_CH_EN_Y                                     ((uint8_t) 0x02)
#define MAG_CH_EN_XY                                    ((uint8_t) 0x03)
#define MAG_CH_EN_Z                                     ((uint8_t) 0x04)
#define MAG_CH_EN_ZX                                    ((uint8_t) 0x05)
#define MAG_CH_EN_YZ                                    ((uint8_t) 0x06)
#define MAG_CH_EN_XYZ                                   ((uint8_t) 0x07)
#define MAG_CH_EN_XYX                                   ((uint8_t) 0x08)
#define MAG_CH_EN_YXY                                   ((uint8_t) 0x09)
#define MAG_CH_EN_YZY                                   ((uint8_t) 0x0A)
#define MAG_CH_EN_XZX                                   ((uint8_t) 0x0B)
#define MAG_CH_EN_XYZPositiveDiagnosticOffset           ((uint8_t) 0x0C)    /* TMAG5173 Only */
#define MAG_CH_EN_XYZNegativeDiagnosticOffset           ((uint8_t) 0x0D)    /* TMAG5173 Only */
#define MAG_CH_EN_HallResistanceADCCheck                ((uint8_t) 0x0E)    /* TMAG5173 Only */
#define MAG_CH_EN_HallOffsetAFECheck                    ((uint8_t) 0x0F)    /* TMAG5173 Only */

#define MAG_THR_DIR_AboveThreshold                      ((uint8_t) 0x00)
#define MAG_THR_DIR_BelowThreshold                      ((uint8_t) 0x01)

#define THRX_COUNT_One                                  ((uint8_t) 0x00)
#define THRX_COUNT_Four                                 ((uint8_t) 0x01)

#define ANGLE_EN_Disabled                               ((uint8_t) 0x00)
#define ANGLE_EN_X1stY2nd                               ((uint8_t) 0x01)
#define ANGLE_EN_Y1stZ2nd                               ((uint8_t) 0x02)
#define ANGLE_EN_X1stZ2nd                               ((uint8_t) 0x03)

#define CONV_AVG_1xAverage                              ((uint8_t) 0x00)
#define CONV_AVG_2xAverage                              ((uint8_t) 0x01)
#define CONV_AVG_4xAverage                              ((uint8_t) 0x02)
#define CONV_AVG_8xAverage                              ((uint8_t) 0x03)
#define CONV_AVG_16xAverage                             ((uint8_t) 0x04)
#define CONV_AVG_32xAverage                             ((uint8_t) 0x05)

#define THR_HYST_2sComplement                           ((uint8_t) 0x00)
#define THR_HYST_7LSBBits                               ((uint8_t) 0x01)
#define THR_HYST_8LSB                                   ((uint8_t) 0x02)    /* TMAG5173 Only */
#define THR_HYST_16LSB                                  ((uint8_t) 0x03)    /* TMAG5173 Only */
#define THR_HYST_32LSB                                  ((uint8_t) 0x04)    /* TMAG5173 Only */
#define THR_HYST_64LSB                                  ((uint8_t) 0x05)    /* TMAG5173 Only */
#define THR_HYST_128LSB                                 ((uint8_t) 0x06)    /* TMAG5173 Only */
#define THR_HYST_256LSB                                 ((uint8_t) 0x07)    /* TMAG5173 Only */

#define Set_X_Threshold                                 ((uint8_t) 0x00)
#define Set_Y_Threshold                                 ((uint8_t) 0x01)
#define Set_Z_Threshold                                 ((uint8_t) 0x02)
#define Set_T_Threshold                                 ((uint8_t) 0x03)

#define MAG_TEMPCO_None                                 ((uint8_t) 0x00)
#define MAG_TEMPCO_NdBFe                                ((uint8_t) 0x01)
#define MAG_TEMPCO_SmCo                                 ((uint8_t) 0x02)    /* TMAG5173 Only */
#define MAG_TEMPCO_Ceramic                              ((uint8_t) 0x03)

#define RANGE_40mTor133mT                               ((uint8_t) 0x00)
#define RANGE_80mTor266mT                               ((uint8_t) 0x01)

/*
 * Temperature Electrical Characteristics (ECHAR)
 *
 * Currently the 'Typical' Electrical Characteristics (ECHAR) of the device are set for
 * ECHAR_T_ADC_RES and ECHAR_T_SENS_T0. These values can differ and, if through device
 * calibration their actual values for a particular device are found, can be updated for
 * more accurate temperature measurement.
 *
 * Pg. 6 (TMAG5273) / Pg. 5 (TMAG5173) of the datasheet contains the descriptions of
 * the Temperature Sensing Electrical Characteristics for the TMAG5x73. The definition
 * names match their written counterparts.
 */

// TEMP_RESULT decimal value @ ECHAR_T_SENS_T0
#define ECHAR_T_ADC_T0                                  ((float) 17508)

// Reference Temperature for ECHAR_T_ADC_T0 (C)
#define ECHAR_T_SENS_T0                                 ((float) 25) // Typical value provided datasheet, actual can differ

// Temp sensing resolution (LSB/C)
#define ECHAR_T_ADC_RES                                 ((float) 60.1) // Typical value provided by datasheet, actual can differ



//**********************************************************************************
//
// Register definitions
//
//**********************************************************************************


/* Register 0x00 (DEVICE_CONFIG_1) definition
 * |-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
 * |         Bit 7         |         Bit 6         |         Bit 5         |         Bit 4         |         Bit 3         |         Bit 2         |         Bit 1         |         Bit 0         |
 * |-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
 * |         CRC_EN        |                MAG_TEMPCO[1:0]                |                             CONV_AVG[2:0]                             |                  I2C_RD[1:0]                  |
 * |-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
 */

/* DEVICE_CONFIG_1 register address */
    #define DEVICE_CONFIG_1_ADDRESS                                         ((uint8_t) 0x00)

    /* DEVICE_CONFIG_1 default (reset) value */
    #define DEVICE_CONFIG_1_DEFAULT                                         ((uint8_t) 0x00)

    /* DEVICE_CONFIG_1 register field masks */
    #define DEVICE_CONFIG_1_CRC_EN_MASK                                     ((uint8_t) 0x80)
    #define DEVICE_CONFIG_1_MAG_TEMPCO_MASK                                 ((uint8_t) 0x60)
    #define DEVICE_CONFIG_1_CONV_AVG_MASK                                   ((uint8_t) 0x1C)
    #define DEVICE_CONFIG_1_I2C_RD_MASK                                     ((uint8_t) 0x03)

    /* CRC_EN field values */
    #define DEVICE_CONFIG_1_CRC_EN_Disabled                                 ((uint8_t) 0x00)
    #define DEVICE_CONFIG_1_CRC_EN_Enabled                                  ((uint8_t) 0x80)

    /* MAG_TEMPCO field values */
    #define DEVICE_CONFIG_1_MAG_TEMPCO_None                                 ((uint8_t) 0x00)
    #define DEVICE_CONFIG_1_MAG_TEMPCO_NdBFe                                ((uint8_t) 0x20)
    #define DEVICE_CONFIG_1_MAG_TEMPCO_SmCo                                 ((uint8_t) 0x40)    /* TMAG5173 Only */
    #define DEVICE_CONFIG_1_MAG_TEMPCO_Ceramic                              ((uint8_t) 0x60)

    /* CONV_AVG field values */
    #define DEVICE_CONFIG_1_CONV_AVG_1xAverage                              ((uint8_t) 0x00)
    #define DEVICE_CONFIG_1_CONV_AVG_2xAverage                              ((uint8_t) 0x04)
    #define DEVICE_CONFIG_1_CONV_AVG_4xAverage                              ((uint8_t) 0x08)
    #define DEVICE_CONFIG_1_CONV_AVG_8xAverage                              ((uint8_t) 0x0C)
    #define DEVICE_CONFIG_1_CONV_AVG_16xAverage                             ((uint8_t) 0x10)
    #define DEVICE_CONFIG_1_CONV_AVG_32xAverage                             ((uint8_t) 0x14)

    /* I2C_RD field values */
    #define DEVICE_CONFIG_1_I2C_RD_StandardI2C                              ((uint8_t) 0x00)
    #define DEVICE_CONFIG_1_I2C_RD_SensorDataStatus                         ((uint8_t) 0x01)
    #define DEVICE_CONFIG_1_I2C_RD_SensorMSBDataStatus                      ((uint8_t) 0x02)
    #define DEVICE_CONFIG_1_I2C_RD_Reserved                                 ((uint8_t) 0x03)



/* Register 0x01 (DEVICE_CONFIG_2) definition
 * |-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
 * |         Bit 7         |         Bit 6         |         Bit 5         |         Bit 4         |         Bit 3         |         Bit 2         |         Bit 1         |         Bit 0         |
 * |-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
 * |                             THR_HYST[2:0]                             |         LP_LN         |   I2C_GLITCH_FILTER   |      TRIGGER_MODE     |              OPERATING_MODE[1:0]              |
 * |-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
 */

/* DEVICE_CONFIG_2 register address */
    #define DEVICE_CONFIG_2_ADDRESS                                         ((uint8_t) 0x01)

    /* DEVICE_CONFIG_2 default (reset) value */
    #define DEVICE_CONFIG_2_DEFAULT                                         ((uint8_t) 0x00)

    /* DEVICE_CONFIG_2 register field masks */
    #define DEVICE_CONFIG_2_THR_HYST_MASK                                   ((uint8_t) 0xE0)
    #define DEVICE_CONFIG_2_LP_LN_MASK                                      ((uint8_t) 0x10)
    #define DEVICE_CONFIG_2_I2C_GLITCH_FILTER_MASK                          ((uint8_t) 0x08)
    #define DEVICE_CONFIG_2_TRIGGER_MODE_MASK                               ((uint8_t) 0x04)
    #define DEVICE_CONFIG_2_OPERATING_MODE_MASK                             ((uint8_t) 0x03)

    /* THR_HYST field values */
    #define DEVICE_CONFIG_2_THR_HYST_2sComplement                           ((uint8_t) 0x00)
    #define DEVICE_CONFIG_2_THR_HYST_7LSBBits                               ((uint8_t) 0x20)
    #define DEVICE_CONFIG_2_THR_HYST_8LSB                                   ((uint8_t) 0x40)    /* TMAG5173 Only */
    #define DEVICE_CONFIG_2_THR_HYST_16LSB                                  ((uint8_t) 0x60)    /* TMAG5173 Only */
    #define DEVICE_CONFIG_2_THR_HYST_32LSB                                  ((uint8_t) 0x80)    /* TMAG5173 Only */
    #define DEVICE_CONFIG_2_THR_HYST_64LSB                                  ((uint8_t) 0xA0)    /* TMAG5173 Only */
    #define DEVICE_CONFIG_2_THR_HYST_128LSB                                 ((uint8_t) 0xC0)    /* TMAG5173 Only */
    #define DEVICE_CONFIG_2_THR_HYST_256LSB                                 ((uint8_t) 0xE0)    /* TMAG5173 Only */

    /* LP_LN field values */
    #define DEVICE_CONFIG_2_LP_LN_LowActiveCurrent                          ((uint8_t) 0x00)
    #define DEVICE_CONFIG_2_LP_LN_LowNoise                                  ((uint8_t) 0x10)

    /* I2C_GLITCH_FILTER field values */
    #define DEVICE_CONFIG_2_I2C_GLITCH_FILTER_On                            ((uint8_t) 0x00)
    #define DEVICE_CONFIG_2_I2C_GLITCH_FILTER_Off                           ((uint8_t) 0x08)

    /* TRIGGER_MODE field values */
    #define DEVICE_CONFIG_2_TRIGGER_MODE_I2CCommandBits                     ((uint8_t) 0x00)
    #define DEVICE_CONFIG_2_TRIGGER_MODE_TriggerSignalatINTPin              ((uint8_t) 0x04)

    /* OPERATING_MODE field values */
    #define DEVICE_CONFIG_2_OPERATING_MODE_Standby                          ((uint8_t) 0x00)
    #define DEVICE_CONFIG_2_OPERATING_MODE_Sleep                            ((uint8_t) 0x01)
    #define DEVICE_CONFIG_2_OPERATING_MODE_ContinuousMeasure                ((uint8_t) 0x02)
    #define DEVICE_CONFIG_2_OPERATING_MODE_WakeupandSleep                   ((uint8_t) 0x03)



/* Register 0x02 (SENSOR_CONFIG_1) definition
 * |-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
 * |         Bit 7         |         Bit 6         |         Bit 5         |         Bit 4         |         Bit 3         |         Bit 2         |         Bit 1         |         Bit 0         |
 * |-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
 * |                                         MAG_CH_EN[3:0]                                        |                                         SLEEPTIME[3:0]                                        |
 * |-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
 */

/* SENSOR_CONFIG_1 register address */
    #define SENSOR_CONFIG_1_ADDRESS                                         ((uint8_t) 0x02)

    /* SENSOR_CONFIG_1 default (reset) value */
    #define SENSOR_CONFIG_1_DEFAULT                                         ((uint8_t) 0x00)

    /* SENSOR_CONFIG_1 register field masks */
    #define SENSOR_CONFIG_1_MAG_CH_EN_MASK                                  ((uint8_t) 0xF0)
    #define SENSOR_CONFIG_1_SLEEPTIME_MASK                                  ((uint8_t) 0x0F)

    /* MAG_CH_EN field values */
    #define SENSOR_CONFIG_1_MAG_CH_EN_Off                                   ((uint8_t) 0x00)
    #define SENSOR_CONFIG_1_MAG_CH_EN_X                                     ((uint8_t) 0x10)
    #define SENSOR_CONFIG_1_MAG_CH_EN_Y                                     ((uint8_t) 0x20)
    #define SENSOR_CONFIG_1_MAG_CH_EN_XY                                    ((uint8_t) 0x30)
    #define SENSOR_CONFIG_1_MAG_CH_EN_Z                                     ((uint8_t) 0x40)
    #define SENSOR_CONFIG_1_MAG_CH_EN_ZX                                    ((uint8_t) 0x50)
    #define SENSOR_CONFIG_1_MAG_CH_EN_YZ                                    ((uint8_t) 0x60)
    #define SENSOR_CONFIG_1_MAG_CH_EN_XYZ                                   ((uint8_t) 0x70)
    #define SENSOR_CONFIG_1_MAG_CH_EN_XYX                                   ((uint8_t) 0x80)
    #define SENSOR_CONFIG_1_MAG_CH_EN_YXY                                   ((uint8_t) 0x90)
    #define SENSOR_CONFIG_1_MAG_CH_EN_YZY                                   ((uint8_t) 0xA0)
    #define SENSOR_CONFIG_1_MAG_CH_EN_XZX                                   ((uint8_t) 0xB0)
    #define SENSOR_CONFIG_1_MAG_CH_EN_XYZPositiveDiagnosticOffset           ((uint8_t) 0xC0)    /* TMAG5173 Only */
    #define SENSOR_CONFIG_1_MAG_CH_EN_XYZNegativeDiagnosticOffset           ((uint8_t) 0xD0)    /* TMAG5173 Only */
    #define SENSOR_CONFIG_1_MAG_CH_EN_HallResistanceADCCheck                ((uint8_t) 0xE0)    /* TMAG5173 Only */
    #define SENSOR_CONFIG_1_MAG_CH_EN_HallOffsetAFECheck                    ((uint8_t) 0xF0)    /* TMAG5173 Only */

    /* SLEEPTIME field values */
    #define SENSOR_CONFIG_1_SLEEPTIME_1ms                                   ((uint8_t) 0x00)
    #define SENSOR_CONFIG_1_SLEEPTIME_5ms                                   ((uint8_t) 0x01)
    #define SENSOR_CONFIG_1_SLEEPTIME_10ms                                  ((uint8_t) 0x02)
    #define SENSOR_CONFIG_1_SLEEPTIME_15ms                                  ((uint8_t) 0x03)
    #define SENSOR_CONFIG_1_SLEEPTIME_20ms                                  ((uint8_t) 0x04)
    #define SENSOR_CONFIG_1_SLEEPTIME_30ms                                  ((uint8_t) 0x05)
    #define SENSOR_CONFIG_1_SLEEPTIME_50ms                                  ((uint8_t) 0x06)
    #define SENSOR_CONFIG_1_SLEEPTIME_100ms                                 ((uint8_t) 0x07)
    #define SENSOR_CONFIG_1_SLEEPTIME_500ms                                 ((uint8_t) 0x08)
    #define SENSOR_CONFIG_1_SLEEPTIME_1000ms                                ((uint8_t) 0x09)
    #define SENSOR_CONFIG_1_SLEEPTIME_2000ms                                ((uint8_t) 0x0A)
    #define SENSOR_CONFIG_1_SLEEPTIME_5000ms                                ((uint8_t) 0x0B)
    #define SENSOR_CONFIG_1_SLEEPTIME_20000ms                               ((uint8_t) 0x0C)



/* Register 0x03 (SENSOR_CONFIG_2) definition
 * |-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
 * |         Bit 7         |         Bit 6         |         Bit 5         |         Bit 4         |         Bit 3         |         Bit 2         |         Bit 1         |         Bit 0         |
 * |-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
 * |           0           |       THRX_COUNT      |      MAG_THR_DIR      |      MAG_GAIN_CH      |                 ANGLE_EN[1:0]                 |       X_Y_RANGE       |        Z_RANGE        |
 * |-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
 */

    /* SENSOR_CONFIG_2 register address */
    #define SENSOR_CONFIG_2_ADDRESS                                         ((uint8_t) 0x03)

    /* SENSOR_CONFIG_2 default (reset) value */
    #define SENSOR_CONFIG_2_DEFAULT                                         ((uint8_t) 0x00)

    /* SENSOR_CONFIG_2 register field masks */
    #define SENSOR_CONFIG_2_THRX_COUNT_MASK                                 ((uint8_t) 0x40)
    #define SENSOR_CONFIG_2_MAG_THR_DIR_MASK                                ((uint8_t) 0x20)
    #define SENSOR_CONFIG_2_MAG_GAIN_CH_MASK                                ((uint8_t) 0x10)
    #define SENSOR_CONFIG_2_ANGLE_EN_MASK                                   ((uint8_t) 0x0C)
    #define SENSOR_CONFIG_2_X_Y_RANGE_MASK                                  ((uint8_t) 0x02)
    #define SENSOR_CONFIG_2_Z_RANGE_MASK                                    ((uint8_t) 0x01)

    /* THRX_COUNT field values */
    #define SENSOR_CONFIG_2_THRX_COUNT_One                                  ((uint8_t) 0x00)
    #define SENSOR_CONFIG_2_THRX_COUNT_Four                                 ((uint8_t) 0x40)

    /* MAG_THR_DIR field values */
    #define SENSOR_CONFIG_2_MAG_THR_DIR_AboveThreshold                      ((uint8_t) 0x00)
    #define SENSOR_CONFIG_2_MAG_THR_DIR_BelowThreshold                      ((uint8_t) 0x20)

    /* MAG_GAIN_CH field values */
    #define SENSOR_CONFIG_2_MAG_GAIN_CH_1stChannel                          ((uint8_t) 0x00)
    #define SENSOR_CONFIG_2_MAG_GAIN_CH_2ndChannel                          ((uint8_t) 0x10)

    /* ANGLE_EN field values */
    #define SENSOR_CONFIG_2_ANGLE_EN_Disabled                               ((uint8_t) 0x00)
    #define SENSOR_CONFIG_2_ANGLE_EN_X1stY2nd                               ((uint8_t) 0x04)
    #define SENSOR_CONFIG_2_ANGLE_EN_Y1stZ2nd                               ((uint8_t) 0x08)
    #define SENSOR_CONFIG_2_ANGLE_EN_X1stZ2nd                               ((uint8_t) 0x0C)

    /* X_Y_RANGE field values */
    #define SENSOR_CONFIG_2_X_Y_RANGE_40mTor133mT                           ((uint8_t) 0x00)
    #define SENSOR_CONFIG_2_X_Y_RANGE_80mTor266mT                           ((uint8_t) 0x02)

    /* Z_RANGE field values */
    #define SENSOR_CONFIG_2_Z_RANGE_40mTor133mT                             ((uint8_t) 0x00)
    #define SENSOR_CONFIG_2_Z_RANGE_80mTor266mT                             ((uint8_t) 0x01)



/* Register 0x04 (X_THR_CONFIG) definition
 * |-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
 * |         Bit 7         |         Bit 6         |         Bit 5         |         Bit 4         |         Bit 3         |         Bit 2         |         Bit 1         |         Bit 0         |
 * |-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
 * |                                                                                       X_THR_CONFIG[7:0]                                                                                       |
 * |-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
 */

    /* X_THR_CONFIG register address */
    #define X_THR_CONFIG_ADDRESS                                            ((uint8_t) 0x04)

    /* X_THR_CONFIG default (reset) value */
    #define X_THR_CONFIG_DEFAULT                                            ((uint8_t) 0x00)

    /* X_THR_CONFIG register field masks */
    #define X_THR_CONFIG_X_THR_CONFIG_MASK                                  ((uint8_t) 0xFF)



/* Register 0x05 (Y_THR_CONFIG) definition
 * |-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
 * |         Bit 7         |         Bit 6         |         Bit 5         |         Bit 4         |         Bit 3         |         Bit 2         |         Bit 1         |         Bit 0         |
 * |-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
 * |                                                                                       Y_THR_CONFIG[7:0]                                                                                       |
 * |-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
 */

    /* Y_THR_CONFIG register address */
    #define Y_THR_CONFIG_ADDRESS                                            ((uint8_t) 0x05)

    /* Y_THR_CONFIG default (reset) value */
    #define Y_THR_CONFIG_DEFAULT                                            ((uint8_t) 0x00)

    /* Y_THR_CONFIG register field masks */
    #define Y_THR_CONFIG_Y_THR_CONFIG_MASK                                  ((uint8_t) 0xFF)



/* Register 0x06 (Z_THR_CONFIG) definition
 * |-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
 * |         Bit 7         |         Bit 6         |         Bit 5         |         Bit 4         |         Bit 3         |         Bit 2         |         Bit 1         |         Bit 0         |
 * |-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
 * |                                                                                       Z_THR_CONFIG[7:0]                                                                                       |
 * |-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
 */

    /* Z_THR_CONFIG register address */
    #define Z_THR_CONFIG_ADDRESS                                            ((uint8_t) 0x06)

    /* Z_THR_CONFIG default (reset) value */
    #define Z_THR_CONFIG_DEFAULT                                            ((uint8_t) 0x00)

    /* Z_THR_CONFIG register field masks */
    #define Z_THR_CONFIG_Z_THR_CONFIG_MASK                                  ((uint8_t) 0xFF)



/* Register 0x07 (T_CONFIG) definition
 * |-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
 * |         Bit 7         |         Bit 6         |         Bit 5         |         Bit 4         |         Bit 3         |         Bit 2         |         Bit 1         |         Bit 0         |
 * |-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
 * |                                                                           T_THR_CONFIG[6:0]                                                                           |        T_CH_EN        |
 * |-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
 */

    /* T_CONFIG register address */
    #define T_CONFIG_ADDRESS                                                ((uint8_t) 0x07)

    /* T_CONFIG default (reset) value */
    #define T_CONFIG_DEFAULT                                                ((uint8_t) 0x00)

    /* T_CONFIG register field masks */
    #define T_CONFIG_T_THR_CONFIG_MASK                                      ((uint8_t) 0xFE)
    #define T_CONFIG_T_CH_EN_MASK                                           ((uint8_t) 0x01)

/* T_CH_EN field values */
    #define T_CONFIG_T_CH_EN_Disabled                                       ((uint8_t) 0x00)
    #define T_CONFIG_T_CH_EN_Enabled                                        ((uint8_t) 0x01)



/* Register 0x08 (INT_CONFIG_1) definition
 * |-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
 * |         Bit 7         |         Bit 6         |         Bit 5         |         Bit 4         |         Bit 3         |         Bit 2         |         Bit 1         |         Bit 0         |
 * |-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
 * |        RSLT_INT       |       THRSLD_INT      |       INT_STATE       |                             INT_MODE[2:0]                             |           0           |       MASK_INTB       |
 * |-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
 */

/* INT_CONFIG_1 register address */
    #define INT_CONFIG_1_ADDRESS                                            ((uint8_t) 0x08)

    /* INT_CONFIG_1 default (reset) value */
    #define INT_CONFIG_1_DEFAULT                                            ((uint8_t) 0x00)

    /* INT_CONFIG_1 register field masks */
    #define INT_CONFIG_1_RSLT_INT_MASK                                      ((uint8_t) 0x80)
    #define INT_CONFIG_1_THRSLD_INT_MASK                                    ((uint8_t) 0x40)
    #define INT_CONFIG_1_INT_STATE_MASK                                     ((uint8_t) 0x20)
    #define INT_CONFIG_1_INT_MODE_MASK                                      ((uint8_t) 0x1C)
    #define INT_CONFIG_1_MASK_INTB_MASK                                     ((uint8_t) 0x01)

    /* RSLT_INT field values */
    #define INT_CONFIG_1_RSLT_INT_Disabled                                  ((uint8_t) 0x00)
    #define INT_CONFIG_1_RSLT_INT_Enabled                                   ((uint8_t) 0x80)

    /* THRSLD_INT field values */
    #define INT_CONFIG_1_THRSLD_INT_Disabled                                ((uint8_t) 0x00)
    #define INT_CONFIG_1_THRSLD_INT_Enabled                                 ((uint8_t) 0x40)

    /* INT_STATE field values */
    #define INT_CONFIG_1_INT_STATE_UntilClear                               ((uint8_t) 0x00)
    #define INT_CONFIG_1_INT_STATE_Pulse                                    ((uint8_t) 0x20)

    /* INT_MODE field values */
    #define INT_CONFIG_1_INT_MODE_Disabled                                  ((uint8_t) 0x00)
    #define INT_CONFIG_1_INT_MODE_INT                                       ((uint8_t) 0x04)
    #define INT_CONFIG_1_INT_MODE_INTWhenI2CBusFree                         ((uint8_t) 0x08)
    #define INT_CONFIG_1_INT_MODE_SCL                                       ((uint8_t) 0x0C)
    #define INT_CONFIG_1_INT_MODE_SCLWhenI2CBusFree                         ((uint8_t) 0x10)
    #define INT_CONFIG_1_INT_MODE_UnipolarSwitchMode                        ((uint8_t) 0x14)    /* TMAG5173 Only */
    #define INT_CONFIG_1_INT_MODE_OmnipolarSwitchMode                       ((uint8_t) 0x18)    /* TMAG5173 Only */
    #define INT_CONFIG_1_INT_MODE_DefaultDisabled                           ((uint8_t) 0x1C)    /* TMAG5173 Only */

    /* MASK_INTB field values */
    #define INT_CONFIG_1_MASK_INTB_Enabled                                  ((uint8_t) 0x00)
    #define INT_CONFIG_1_MASK_INTB_Disabled                                 ((uint8_t) 0x01)



/* Register 0x09 (MAG_GAIN_CONFIG) definition
 * |-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
 * |         Bit 7         |         Bit 6         |         Bit 5         |         Bit 4         |         Bit 3         |         Bit 2         |         Bit 1         |         Bit 0         |
 * |-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
 * |                                                                                        GAIN_VALUE[7:0]                                                                                        |
 * |-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
 */

    /* MAG_GAIN_CONFIG register address */
    #define MAG_GAIN_CONFIG_ADDRESS                                         ((uint8_t) 0x09)

    /* MAG_GAIN_CONFIG default (reset) value */
    #define MAG_GAIN_CONFIG_DEFAULT                                         ((uint8_t) 0x00)

    /* MAG_GAIN_CONFIG register field masks */
    #define MAG_GAIN_CONFIG_GAIN_VALUE_MASK                                 ((uint8_t) 0xFF)



/* Register 0x0A (MAG_OFFSET_CONFIG_1) definition
 * |-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
 * |         Bit 7         |         Bit 6         |         Bit 5         |         Bit 4         |         Bit 3         |         Bit 2         |         Bit 1         |         Bit 0         |
 * |-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
 * |                                                                                     OFFSET_VALUE_1ST[7:0]                                                                                     |
 * |-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
 */

    /* MAG_OFFSET_CONFIG_1 register address */
    #define MAG_OFFSET_CONFIG_1_ADDRESS                                     ((uint8_t) 0x0A)

    /* MAG_OFFSET_CONFIG_1 default (reset) value */
    #define MAG_OFFSET_CONFIG_1_DEFAULT                                     ((uint8_t) 0x00)

    /* MAG_OFFSET_CONFIG_1 register field masks */
    #define MAG_OFFSET_CONFIG_1_OFFSET_VALUE_1ST_MASK                       ((uint8_t) 0xFF)



/* Register 0x0B (MAG_OFFSET_CONFIG_2) definition
 * |-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
 * |         Bit 7         |         Bit 6         |         Bit 5         |         Bit 4         |         Bit 3         |         Bit 2         |         Bit 1         |         Bit 0         |
 * |-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
 * |                                                                                     OFFSET_VALUE_2ND[7:0]                                                                                     |
 * |-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
 */

    /* MAG_OFFSET_CONFIG_2 register address */
    #define MAG_OFFSET_CONFIG_2_ADDRESS                                     ((uint8_t) 0x0B)

    /* MAG_OFFSET_CONFIG_2 default (reset) value */
    #define MAG_OFFSET_CONFIG_2_DEFAULT                                     ((uint8_t) 0x00)

    /* MAG_OFFSET_CONFIG_2 register field masks */
    #define MAG_OFFSET_CONFIG_2_OFFSET_VALUE_2ND_MASK                       ((uint8_t) 0xFF)



/* Register 0x0C (I2C_ADDRESS) definition
 * |-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
 * |         Bit 7         |         Bit 6         |         Bit 5         |         Bit 4         |         Bit 3         |         Bit 2         |         Bit 1         |         Bit 0         |
 * |-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
 * |                                                                            I2C_ADDRESS[6:0]                                                                           | I2C_ADDRESS_UPDATE_EN |
 * |-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
 */

/* I2C_ADDRESS register address */
    #define I2C_ADDRESS_ADDRESS                                             ((uint8_t) 0x0C)

    /* I2C_ADDRESS default (reset) value */
    #define I2C_ADDRESS_DEFAULT                                             ((uint8_t) 0x6A)

    /* I2C_ADDRESS register field masks */
    #define I2C_ADDRESS_I2C_ADDRESS_MASK                                    ((uint8_t) 0xFE)
    #define I2C_ADDRESS_I2C_ADDRESS_UPDATE_EN_MASK                          ((uint8_t) 0x01)

    /* I2C_ADDRESS_UPDATE_EN field values */
    #define I2C_ADDRESS_I2C_ADDRESS_UPDATE_EN_Disabled                      ((uint8_t) 0x00)
    #define I2C_ADDRESS_I2C_ADDRESS_UPDATE_EN_Enabled                       ((uint8_t) 0x01)



/* Register 0x0D (DEVICE_ID) definition
 * |-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
 * |         Bit 7         |         Bit 6         |         Bit 5         |         Bit 4         |         Bit 3         |         Bit 2         |         Bit 1         |         Bit 0         |
 * |-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
 * |           0           |           0           |           0           |           0           |           0           |           0           |                    VER[1:0]                   |
 * |-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
 */

    /* DEVICE_ID register address */
    #define DEVICE_ID_ADDRESS                                               ((uint8_t) 0x0D)

    /* DEVICE_ID default (reset) value */
    #define DEVICE_ID_DEFAULT                                               ((uint8_t) 0x01)

    /* DEVICE_ID register field masks */
    #define DEVICE_ID_VER_MASK                                              ((uint8_t) 0x03)

    /* VER field values */
    #define DEVICE_ID_VER_40mTand80mTRangeOption0                           ((uint8_t) 0x00)
    #define DEVICE_ID_VER_40mTand80mTRange                                  ((uint8_t) 0x01)
    #define DEVICE_ID_VER_133mTand266mTRange                                ((uint8_t) 0x02)
    #define DEVICE_ID_VER_Reserved1                                         ((uint8_t) 0x03)



/* Register 0x0E (MANUFACTURER_ID_LSB) definition
 * |-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
 * |         Bit 7         |         Bit 6         |         Bit 5         |         Bit 4         |         Bit 3         |         Bit 2         |         Bit 1         |         Bit 0         |
 * |-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
 * |                                                                                    MANUFACTURER_ID_LSB[7:0]                                                                                   |
 * |-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
 */

    /* MANUFACTURER_ID_LSB register address */
    #define MANUFACTURER_ID_LSB_ADDRESS                                     ((uint8_t) 0x0E)

    /* MANUFACTURER_ID_LSB default (reset) value */
    #define MANUFACTURER_ID_LSB_DEFAULT                                     ((uint8_t) 0x49)

    /* MANUFACTURER_ID_LSB register field masks */
    #define MANUFACTURER_ID_LSB_MANUFACTURER_ID_LSB_MASK                    ((uint8_t) 0xFF)



/* Register 0x0F (MANUFACTURER_ID_MSB) definition
 * |-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
 * |         Bit 7         |         Bit 6         |         Bit 5         |         Bit 4         |         Bit 3         |         Bit 2         |         Bit 1         |         Bit 0         |
 * |-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
 * |                                                                                    MANUFACTURER_ID_MSB[7:0]                                                                                   |
 * |-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
 */

    /* MANUFACTURER_ID register address */
    #define MANUFACTURER_ID_MSB_ADDRESS                                     ((uint8_t) 0x0F)

    /* MANUFACTURER_ID default (reset) value */
    #define MANUFACTURER_ID_MSB_DEFAULT                                     ((uint8_t) 0x54)

    /* MANUFACTURER_ID register field masks */
    #define MANUFACTURER_ID_MANUFACTURER_ID_MSB_MASK                        ((uint8_t) 0xFF)



/* Register 0x10 (T_MSB_RESULT) definition
 * |-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
 * |         Bit 7         |         Bit 6         |         Bit 5         |         Bit 4         |         Bit 3         |         Bit 2         |         Bit 1         |         Bit 0         |
 * |-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
 * |                                                                                      T_CH_RESULT_MSB[7:0]                                                                                     |
 * |-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
 */

    /* T_MSB_RESULT register address */
    #define T_MSB_RESULT_ADDRESS                                            ((uint8_t) 0x10)

    /* T_MSB_RESULT default (reset) value */
    #define T_MSB_RESULT_DEFAULT                                            ((uint8_t) 0x00)

    /* T_MSB_RESULT register field masks */
    #define T_MSB_RESULT_T_CH_RESULT_MSB_MASK                               ((uint8_t) 0xFF)



/* Register 0x11 (T_LSB_RESULT) definition
 * |-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
 * |         Bit 7         |         Bit 6         |         Bit 5         |         Bit 4         |         Bit 3         |         Bit 2         |         Bit 1         |         Bit 0         |
 * |-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
 * |                                                                                      T_CH_RESULT_LSB[7:0]                                                                                     |
 * |-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
 */

    /* T_LSB_RESULT register address */
    #define T_LSB_RESULT_ADDRESS                                            ((uint8_t) 0x11)

    /* T_LSB_RESULT default (reset) value */
    #define T_LSB_RESULT_DEFAULT                                            ((uint8_t) 0x00)

    /* T_LSB_RESULT register field masks */
    #define T_LSB_RESULT_T_CH_RESULT_LSB_MASK                               ((uint8_t) 0xFF)



/* Register 0x12 (X_MSB_RESULT) definition
 * |-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
 * |         Bit 7         |         Bit 6         |         Bit 5         |         Bit 4         |         Bit 3         |         Bit 2         |         Bit 1         |         Bit 0         |
 * |-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
 * |                                                                                      X_CH_RESULT_MSB[7:0]                                                                                     |
 * |-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
 */

    /* X_MSB_RESULT register address */
    #define X_MSB_RESULT_ADDRESS                                            ((uint8_t) 0x12)

    /* X_MSB_RESULT default (reset) value */
    #define X_MSB_RESULT_DEFAULT                                            ((uint8_t) 0x00)

    /* X_MSB_RESULT register field masks */
    #define X_MSB_RESULT_X_CH_RESULT_MSB_MASK                               ((uint8_t) 0xFF)



/* Register 0x13 (X_LSB_RESULT) definition
 * |-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
 * |         Bit 7         |         Bit 6         |         Bit 5         |         Bit 4         |         Bit 3         |         Bit 2         |         Bit 1         |         Bit 0         |
 * |-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
 * |                                                                                      X_CH_RESULT_LSB[7:0]                                                                                     |
 * |-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
 */

    /* X_LSB_RESULT register address */
    #define X_LSB_RESULT_ADDRESS                                            ((uint8_t) 0x13)

    /* X_LSB_RESULT default (reset) value */
    #define X_LSB_RESULT_DEFAULT                                            ((uint8_t) 0x00)

    /* X_LSB_RESULT register field masks */
    #define X_LSB_RESULT_X_CH_RESULT_LSB_MASK                               ((uint8_t) 0xFF)



/* Register 0x14 (Y_MSB_RESULT) definition
 * |-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
 * |         Bit 7         |         Bit 6         |         Bit 5         |         Bit 4         |         Bit 3         |         Bit 2         |         Bit 1         |         Bit 0         |
 * |-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
 * |                                                                                      Y_CH_RESULT_MSB[7:0]                                                                                     |
 * |-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
 */

    /* Y_MSB_RESULT register address */
    #define Y_MSB_RESULT_ADDRESS                                            ((uint8_t) 0x14)

    /* Y_MSB_RESULT default (reset) value */
    #define Y_MSB_RESULT_DEFAULT                                            ((uint8_t) 0x00)

    /* Y_MSB_RESULT register field masks */
    #define Y_MSB_RESULT_Y_CH_RESULT_MSB_MASK                               ((uint8_t) 0xFF)



/* Register 0x15 (Y_LSB_RESULT) definition
 * |-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
 * |         Bit 7         |         Bit 6         |         Bit 5         |         Bit 4         |         Bit 3         |         Bit 2         |         Bit 1         |         Bit 0         |
 * |-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
 * |                                                                                      Y_CH_RESULT_LSB[7:0]                                                                                     |
 * |-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
 */

    /* Y_LSB_RESULT register address */
    #define Y_LSB_RESULT_ADDRESS                                            ((uint8_t) 0x15)

    /* Y_LSB_RESULT default (reset) value */
    #define Y_LSB_RESULT_DEFAULT                                            ((uint8_t) 0x00)

    /* Y_LSB_RESULT register field masks */
    #define Y_LSB_RESULT_Y_CH_RESULT_LSB_MASK                               ((uint8_t) 0xFF)



/* Register 0x16 (Z_MSB_RESULT) definition
 * |-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
 * |         Bit 7         |         Bit 6         |         Bit 5         |         Bit 4         |         Bit 3         |         Bit 2         |         Bit 1         |         Bit 0         |
 * |-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
 * |                                                                                      Z_CH_RESULT_MSB[7:0]                                                                                     |
 * |-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
 */

    /* Z_MSB_RESULT register address */
    #define Z_MSB_RESULT_ADDRESS                                            ((uint8_t) 0x16)

    /* Z_MSB_RESULT default (reset) value */
    #define Z_MSB_RESULT_DEFAULT                                            ((uint8_t) 0x00)

    /* Z_MSB_RESULT register field masks */
    #define Z_MSB_RESULT_Z_CH_RESULT_MSB_MASK                               ((uint8_t) 0xFF)



/* Register 0x17 (Z_LSB_RESULT) definition
 * |-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
 * |         Bit 7         |         Bit 6         |         Bit 5         |         Bit 4         |         Bit 3         |         Bit 2         |         Bit 1         |         Bit 0         |
 * |-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
 * |                                                                                      Z_CH_RESULT_LSB[7:0]                                                                                     |
 * |-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
 */

    /* Z_LSB_RESULT register address */
    #define Z_LSB_RESULT_ADDRESS                                            ((uint8_t) 0x17)

    /* Z_LSB_RESULT default (reset) value */
    #define Z_LSB_RESULT_DEFAULT                                            ((uint8_t) 0x00)

    /* Z_LSB_RESULT register field masks */
    #define Z_LSB_RESULT_Z_CH_RESULT_LSB_MASK                               ((uint8_t) 0xFF)



/* Register 0x18 (CONV_STATUS) definition
 * |-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
 * |         Bit 7         |         Bit 6         |         Bit 5         |         Bit 4         |         Bit 3         |         Bit 2         |         Bit 1         |         Bit 0         |
 * |-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
 * |                             SET_COUNT[2:0]                            |          POR          |           0           |           0           |      DIAG_STATUS      |     RESULT_STATUS     |
 * |-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
 */

/* CONV_STATUS register address */
    #define CONV_STATUS_ADDRESS                                             ((uint8_t) 0x18)

    /* CONV_STATUS default (reset) value */
    #define CONV_STATUS_DEFAULT                                             ((uint8_t) 0x10)

    /* CONV_STATUS register field masks */
    #define CONV_STATUS_SET_COUNT_MASK                                      ((uint8_t) 0xE0)
    #define CONV_STATUS_POR_MASK                                            ((uint8_t) 0x10)
    #define CONV_STATUS_DIAG_STATUS_MASK                                    ((uint8_t) 0x02)
    #define CONV_STATUS_RESULT_STATUS_MASK                                  ((uint8_t) 0x01)

    /* POR field values */
    #define CONV_STATUS_POR_NoPOR                                           ((uint8_t) 0x00)
    #define CONV_STATUS_POR_POROccurred                                     ((uint8_t) 0x10)

    /* DIAG_STATUS field values */
    #define CONV_STATUS_DIAG_STATUS_NoDiagFail                              ((uint8_t) 0x00)
    #define CONV_STATUS_DIAG_STATUS_FailDetected                            ((uint8_t) 0x02)

    /* RESULT_STATUS field values */
    #define CONV_STATUS_RESULT_STATUS_Busy                                  ((uint8_t) 0x00)
    #define CONV_STATUS_RESULT_STATUS_Complete                              ((uint8_t) 0x01)



/* Register 0x19 (ANGLE_RESULT_MSB) definition
 * |-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
 * |         Bit 7         |         Bit 6         |         Bit 5         |         Bit 4         |         Bit 3         |         Bit 2         |         Bit 1         |         Bit 0         |
 * |-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
 * |                                                                                     ANGLE_RESULT_MSB[7:0]                                                                                     |
 * |-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
 */

    /* ANGLE_RESULT_MSB register address */
    #define ANGLE_RESULT_MSB_ADDRESS                                        ((uint8_t) 0x19)

    /* ANGLE_RESULT_MSB default (reset) value */
    #define ANGLE_RESULT_MSB_DEFAULT                                        ((uint8_t) 0x00)

    /* ANGLE_RESULT_MSB register field masks */
    #define ANGLE_RESULT_MSB_ANGLE_RESULT_MSB_MASK                          ((uint8_t) 0xFF)



/* Register 0x1A (ANGLE_RESULT_LSB) definition
 * |-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
 * |         Bit 7         |         Bit 6         |         Bit 5         |         Bit 4         |         Bit 3         |         Bit 2         |         Bit 1         |         Bit 0         |
 * |-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
 * |                                                                                     ANGLE_RESULT_LSB[7:0]                                                                                     |
 * |-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
 */

    /* ANGLE_RESULT_LSB register address */
    #define ANGLE_RESULT_LSB_ADDRESS                                        ((uint8_t) 0x1A)

    /* ANGLE_RESULT_LSB default (reset) value */
    #define ANGLE_RESULT_LSB_DEFAULT                                        ((uint8_t) 0x00)

    /* ANGLE_RESULT_LSB register field masks */
    #define ANGLE_RESULT_LSB_ANGLE_RESULT_LSB_MASK                          ((uint8_t) 0xFF)



/* Register 0x1B (MAGNITUDE_RESULT) definition
 * |-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
 * |         Bit 7         |         Bit 6         |         Bit 5         |         Bit 4         |         Bit 3         |         Bit 2         |         Bit 1         |         Bit 0         |
 * |-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
 * |                                                                                     MAGNITUDE_RESULT[7:0]                                                                                     |
 * |-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
 */

    /* MAGNITUDE_RESULT register address */
    #define MAGNITUDE_RESULT_ADDRESS                                        ((uint8_t) 0x1B)

    /* MAGNITUDE_RESULT default (reset) value */
    #define MAGNITUDE_RESULT_DEFAULT                                        ((uint8_t) 0x00)

    /* MAGNITUDE_RESULT register field masks */
    #define MAGNITUDE_RESULT_MAGNITUDE_RESULT_MASK                          ((uint8_t) 0xFF)



/* Register 0x1C (DEVICE_STATUS) definition
 * |-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
 * |         Bit 7         |         Bit 6         |         Bit 5         |         Bit 4         |         Bit 3         |         Bit 2         |         Bit 1         |         Bit 0         |
 * |-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
 * |           0           |           0           |           0           |        INTB_RB        |         OSC_ER        |         INT_ER        |       OTP_CRC_ER      |       VCC_UV_ER       |
 * |-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
 */

    /* DEVICE_STATUS register address */
    #define DEVICE_STATUS_ADDRESS                                           ((uint8_t) 0x1C)

    /* DEVICE_STATUS default (reset) value */
    #define DEVICE_STATUS_DEFAULT                                           ((uint8_t) 0x10)

    /* DEVICE_STATUS register field masks */
    #define DEVICE_STATUS_INTB_RB_MASK                                      ((uint8_t) 0x10)
    #define DEVICE_STATUS_OSC_ER_MASK                                       ((uint8_t) 0x08)
    #define DEVICE_STATUS_INT_ER_MASK                                       ((uint8_t) 0x04)
    #define DEVICE_STATUS_OTP_CRC_ER_MASK                                   ((uint8_t) 0x02)
    #define DEVICE_STATUS_VCC_UV_ER_MASK                                    ((uint8_t) 0x01)

    /* INTB_RB field values */
    #define DEVICE_STATUS_INTB_RB_INTLow                                    ((uint8_t) 0x00)
    #define DEVICE_STATUS_INTB_RB_INTHigh                                   ((uint8_t) 0x10)

    /* OSC_ER field values */
    #define DEVICE_STATUS_OSC_ER_NoErrorDetected                            ((uint8_t) 0x00)
    #define DEVICE_STATUS_OSC_ER_ErrorDetected                              ((uint8_t) 0x08)

    /* INT_ER field values */
    #define DEVICE_STATUS_INT_ER_NoErrorDetected                            ((uint8_t) 0x00)
    #define DEVICE_STATUS_INT_ER_ErrorDetected                              ((uint8_t) 0x04)

    /* OTP_CRC_ER field values */
    #define DEVICE_STATUS_OTP_CRC_ER_NoErrorDetected                        ((uint8_t) 0x00)
    #define DEVICE_STATUS_OTP_CRC_ER_ErrorDetected                          ((uint8_t) 0x02)

    /* VCC_UV_ER field values */
    #define DEVICE_STATUS_VCC_UV_ER_NoUVDetected                            ((uint8_t) 0x00)
    #define DEVICE_STATUS_VCC_UV_ER_UVDetected                              ((uint8_t) 0x01)



#endif /* TMAG5x73_H_ */
