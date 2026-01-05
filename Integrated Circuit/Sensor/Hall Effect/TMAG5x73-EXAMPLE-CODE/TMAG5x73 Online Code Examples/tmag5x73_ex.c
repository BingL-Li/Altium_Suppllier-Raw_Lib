/**
 * \copyright Copyright (C) 2022 Texas Instruments Incorporated - http://www.ti.com/
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

#include "tmag5x73.h"


//****************************************************************************
//
// Internal variables
//
//****************************************************************************

// These two arrays are used with the CORDIC function to convert the integer output
// into floating point values
const uint32_t magArray[16] = {1518500250,1358187913,1317635818,1307460871,
                               1304914694,1304277995,1304118810,1304079014,
                               1304069065,1304066577,1304065955,1304065800,
                               1304065761,1304065751,1304065749,1304065748};
const uint32_t atanArray32[16] = {536870912,316933406,167458907,85004756,42667331,
                                  21354465,10679838,5340245,2670163,1335087,667544,
                                  333772,166886,83443,41722,20861};



//****************************************************************************
//! Initialization function
//****************************************************************************
void TMAG5x73startup()
{
    // (OPTIONAL) Provide additional delay time for power supply settling
    delay_ms(50);

    TMAG5x73restoreDefaultValues();
}



//****************************************************************************
//! Restore Devices Default Register Values
//!
//! This function sets all of the register to their default values as described
//! in the datasheet in section 7.6
//****************************************************************************
void TMAG5x73restoreDefaultValues()
{
    int i = 0;
    uint8_t defaultRegisters[NUM_REGISTERS] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                                0x00, 0x00, 0x00, 0x00, 0x00, 0x6A, 0x01,
                                                0x49, 0x54, 0x00, 0x00, 0x00, 0x00, 0x00,
                                                0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00,
                                                0x10 };

    for(i = 0; i < NUM_REGISTERS; i++)
    {
        TMAG5x73writeToSingleRegister(i, defaultRegisters[i]);
        delay_us(200000);
    }
}



//****************************************************************************
//! Update I2C Address
//!
//! This function updates the I2C address of the TMAG5x73 which can be useful
//! if there are multiple TMAG5x73 devices on the same I2C bus
//!
//! i2c_address - 7-bit I2C address
//!
//! NOTE: The 7-bit default factory I2C address is loaded from OTP during the
//!       first power up, thus at each power cycle these bits must be written
//!       again to avoid going back to the default factory address
//****************************************************************************
void TMAG5x73updateI2Caddress (uint8_t i2c_address)
{
    uint8_t input;
    // Sets I2C_ADDRESS to i2c_address
    input = TMAG5x73readSingleRegister(I2C_ADDRESS_ADDRESS, Trigger_Conversion_Enabled);
    input &= ~(I2C_ADDRESS_I2C_ADDRESS_MASK);
    input |= (i2c_address << 1) + I2C_ADDRESS_I2C_ADDRESS_UPDATE_EN_Enabled;
    TMAG5x73writeToSingleRegister(I2C_ADDRESS_ADDRESS, input);
}



//****************************************************************************
//****************************************************************************
//
// I2C Functions
//
//****************************************************************************
//****************************************************************************

//****************************************************************************
//! Write to Register Function
//!
//! Takes in the address of the register to edit and the 8-bit frame to overwrite
//! it with and writes the input frame to the register.
//!
//! This function replaces the whole register with the input data, make sure the
//! values desired to be unchanged are in the input data!
//!
//! address       - uint8_t value from 0x00 to 0x1C containing the register
//!                 address to write over
//! data          - the 8-bit frame to be written to the register at address.
//****************************************************************************
void TMAG5x73writeToSingleRegister(uint8_t address, uint8_t data)
{
    // Check that the input address is in range
    assert(address < NUM_REGISTERS);

    // Build TX byte arrays
    uint8_t dataTx[6] = { 0 };

    // MSB is 0 for WRITE command
    dataTx[0] = (address);         // Address + write bit (Start with 0)
    dataTx[1] = (data);

    i2cSendArrays(dataTx, TMAG5x73_FRAME_NUM_BYTES+1); // Number of bytes = Address + data
}



//****************************************************************************
//! Write to Register Function and Trigger Conversion
//!
//! Takes in the address of the register to edit and the 8-bit frame to overwrite
//! it with and writes the input frame to the register. After register address
//! decoding is completed a new conversion will be triggered
//!
//! This function replaces the whole register with the input data, make sure the
//! values desired to be unchanged are in the input data!
//!
//! address       - uint8_t value from 0x00 to 0x1C containing the register address to
//!                 write over
//! data          - the 8-bit frame to be written to the register at address.
//****************************************************************************
void TMAG5x73writeToRegisterWithTrigger(uint8_t address, uint8_t data)
{
    // Check that the input address is in range
    assert(address < NUM_REGISTERS);

    // Build TX byte arrays
    uint8_t dataTx[6] = { 0 };

    //Trigger bit (MSB) is set to 1 for WRITE command
    dataTx[0] = (address | 0x80);  // Address + write bit (Start with 1)
    dataTx[1] = (data);

    i2cSendArrays(dataTx, TMAG5x73_FRAME_NUM_BYTES+1); // Number of bytes = Address + data
}


//****************************************************************************
//! Write to Multiple Registers Function
//!
//! Takes in the first address of the registers to edit, the number of registers to
//! write to, and an array of 8-bit frames to overwrite it with and writes the
//! input frames to the registers.
//!
//! This function replaces the whole register with the input data, make sure the values
//! desired to be unchanged are in the input data!
//!
//! startAddress  - uint8_t value from 0x00 to 0x1C containing the first register
//!                 address to write over
//! count         - the number of registers to write to
//! triggerBit    - sets the trigger bit to start conversion
//!                 0x00 - trigger bit is not set
//!                 0x01 - trigger bit is set
//! data[]        - array containing the 8-bit frames to be written to the registers
//!                 starting at startAddress and continuing until all count registers
//!                 have been written to.
//****************************************************************************
void TMAG5x73writeToMultipleRegisters(uint8_t startAddress, const uint8_t count, uint8_t triggerBit, const uint8_t data[])
{
    // Check that the input address is in range and count is greater than 0
    assert(count > 0);
    assert(startAddress + count <= NUM_REGISTERS);

    // Build TX byte arrays
    uint8_t dataTx[NUM_REGISTERS + 1] = { 0 };

    int i;

    if (triggerBit == 0x01)
        dataTx[0] = (startAddress | 0x80);         // Address + write bit
    else // MSB is 0 for WRITE command
        dataTx[0] = (startAddress); // Address + write bit

    for (i = 1; i <= count; i++)
    {
        dataTx[startAddress + i] = data[i - 1];
    }

    i2cSendArrays(dataTx, count+1);
}



//****************************************************************************
//! Read Single Register Function
//!
//! Takes in an address to read from then creates the dataTx array and calls
//! i2cReceiveArrays function, interpreting dataRx
//!
//! address     - uint8_t value from 0x00 to 0x1C containing the register address to read from
//! triggerBit  - determines if the trigger bit should be set
//!               0x00 - trigger bit is not set
//!               0x01 - trigger bit is set
//****************************************************************************
uint8_t TMAG5x73readSingleRegister(uint8_t address, uint8_t triggerBit)
{
    // Check that the input address is in range
    assert(address < NUM_REGISTERS);

    // Build TX and RX byte arrays
    uint8_t dataTx[6] = { 0 };     // 1 word, each 6 byte long = 6 bytes maximum
    uint8_t dataRx[6] = { 0 };

    // If trigger bit is set MSB is 1 for READ command
    if (triggerBit == 0x01)
        dataTx[0] = (address | 0x80);
    else // Trigger bit is not set and MSB is 0 for READ command
        dataTx[0] = (address);

    dataTx[1] = address;

    i2cReceiveArrays(dataRx, TMAG5x73_FRAME_NUM_BYTES, dataTx);

    return dataRx[0];
}



//****************************************************************************
//! Read Multiple Registers Function
//!
//! Takes in the first address of the registers to read, the number of registers to
//! read, and the trigger bit that will start a conversion after the register address
//! decoding is complete.
//!
//! start_address -  first register address to read from
//!
//! count         -  total number of registers to read
//!
//! triggerBit    - determines if the trigger bit should be set
//!                 0x00 - trigger bit is not set
//!                 0x01 - trigger bit is set
//!
//! data_read_results[] - an empty array that will hold all data read by this function
//****************************************************************************
void TMAG5x73readMultipleRegisters(uint8_t startAddress, uint8_t count, uint8_t triggerBit, uint8_t data_read_results[])
{
    // Check that the input address is in range and count is greater than 0
    assert(count > 0);
    assert((startAddress + count) <= NUM_REGISTERS);

    // Build TX and RX byte arrays
    uint8_t dataTx[NUM_REGISTERS] = { 0 };
    uint8_t dataRx[NUM_REGISTERS] = { 0 };
    int i;

    // If trigger bit is set MSB is 1 for READ command
    if (triggerBit == 0x01)
        dataTx[0] = (startAddress | 0x80);
    else // Trigger bit is not set and MSB is 0 for READ command
        dataTx[0] = (startAddress);

    dataTx[1] = startAddress;

    i2cReceiveArrays(dataRx, count, dataTx);

    for (i = 0; i < count; i++)
            data_read_results[i] = dataRx[i];
}



//****************************************************************************
//! 1-Byte I2C Read Function
//!
//! Takes in the number of data channels that are enabled (T, X, Y, Z), whether
//! or not to do an 8-bit (MSB) or a 16-bit (MSB+LSB) read, and whether or not
//! CRC has been enabled and reads the corresponding registers.
//!
//! numChannels -  number of data channels (T, X, Y, Z) that are enabled
//!                0x01 - One channel enabled
//!                0x02 - Two channels enabled
//!                0x03 - Three channels enabled
//!                0x04 - All channels enabled
//!
//! bitRead     - Data is 8-bit/16-bit
//!               0x00 - 8-bit data read
//!               0x01 - 16-bit data read
//!
//! crc_en      - CRC calculation is enabled
//!               0x00 - CRC calculation is disabled
//!               0x01 - CRC calculation is enabled
//!
//! data_read_results[] - an empty array that will hold all data read by this function
//!
//! NOTE: APPLIES ONLY TO TMAG5273 - When using the 1-byte read command for 16-bit
//!       data, only up to 3 channels data can be sent if CRC is enabled. This
//!       restriction does not apply if CRC is not enabled.
//****************************************************************************
void TMAG5x73oneByteRead(uint8_t numChannels, uint8_t bitRead, uint8_t crc_en, uint8_t data_read_results[])
{
    // Check that the inputs are valid
    assert(numChannels != 0);
    assert(numChannels <= 0x04);
    assert(bitRead <= 0x01);
    assert(crc_en <= 0x01);

    uint8_t count;
    int i;
    // Build RX byte arrays
    uint8_t dataRx[NUM_REGISTERS] = { 0 };

    // Determines the number of registers to read based on whether CRC is enabled,
    // which data channel is enabled, and whether the data read is 8 bits or 16 bits
    if (bitRead == 0x01) // 16-bit data read
    {
        if (crc_en == 0x01) // CRC is enabled
            count = (numChannels * 2) + 2;
        else // CRC is disabled
            count = (numChannels * 2) + 1;

    }
    else // 8-bit data read
    {
        if (crc_en == 0x01) // CRC is enabled
            count = numChannels + 2;
        else // CRC is disabled
            count = numChannels + 1;
    }

    i2cOneByteReceiveArrays(dataRx, count);

    for (i = 0; i < count; i++)
        data_read_results[i] = dataRx[i];
}



//****************************************************************************
//! Set I2C Read Mode
//!
//! read_mode - determines what kind of read mode to select
//!             0x00 = Standard I2C 3-byte read command
//!             0x01 = 1-byte I2C read command for 16-bit sensor data
//!             0x02 = 1-byte I2C read command for 8-bit sensor MSB data
//!
//! NOTE: When using one of the 1-byte read commands, once this mode has been set
//!       any read commands (single, multiple, or otherwise) that follow will only
//!       return one of these result registers in the following order, should
//!       multiple data channels be enabled, T, X, Y, Z.
//****************************************************************************
void TMAG5x73setI2CreadMode(uint8_t read_mode)
{
    // Check that the input is valid
    assert(read_mode <= 0x02);

    uint8_t input;
    // Sets I2C_RD to read_mode
    input = TMAG5x73readSingleRegister(DEVICE_CONFIG_1_ADDRESS, Trigger_Conversion_Enabled);
    input &= ~(DEVICE_CONFIG_1_I2C_RD_MASK);
    input |= DEVICE_CONFIG_1_I2C_RD_StandardI2C + read_mode;
    TMAG5x73writeToSingleRegister(DEVICE_CONFIG_1_ADDRESS, input);
}



//****************************************************************************
//! Enable CRC
//****************************************************************************
void TMAG5x73enableCRC()
{
    uint8_t input;
    // Sets CRC_EN to be enabled (1h)
    input = TMAG5x73readSingleRegister(DEVICE_CONFIG_1_ADDRESS, Trigger_Conversion_Enabled);
    input &= ~(DEVICE_CONFIG_1_CRC_EN_MASK);
    input |= DEVICE_CONFIG_1_CRC_EN_Enabled;
    TMAG5x73writeToSingleRegister(DEVICE_CONFIG_1_ADDRESS, input);
}



//****************************************************************************
//! Disable CRC
//****************************************************************************
void TMAG5x73disableCRC()
{
    uint8_t input;
    // Sets CRC_EN to be disabled (0h)
    input = TMAG5x73readSingleRegister(DEVICE_CONFIG_1_ADDRESS, Trigger_Conversion_Enabled);
    input &= ~(DEVICE_CONFIG_1_CRC_EN_MASK);
    input |= DEVICE_CONFIG_1_CRC_EN_Disabled;
    TMAG5x73writeToSingleRegister(DEVICE_CONFIG_1_ADDRESS, input);
}



//****************************************************************************
//! Turn on I2C Glitch Filter
//****************************************************************************
void TMAG5x73i2cGlitchFilterEnabled()
{
    uint8_t input;
    // Set I2C_GLITCH_FILTER to be enabled (0h)
    input = TMAG5x73readSingleRegister(DEVICE_CONFIG_2_ADDRESS, Trigger_Conversion_Enabled);
    input &= ~(DEVICE_CONFIG_2_I2C_GLITCH_FILTER_MASK);
    input |= DEVICE_CONFIG_2_I2C_GLITCH_FILTER_On;
    TMAG5x73writeToSingleRegister(DEVICE_CONFIG_2_ADDRESS, input);
}



//****************************************************************************
//! Turn off I2C Glitch Filter
//****************************************************************************
void TMAG5x73i2cGlitchFilterDisabled()
{
    uint8_t input;
    // Set I2C_GLITCH_FILTER to be disabled (1h)
    input = TMAG5x73readSingleRegister(DEVICE_CONFIG_2_ADDRESS, Trigger_Conversion_Enabled);
    input &= ~(DEVICE_CONFIG_2_I2C_GLITCH_FILTER_MASK);
    input |= DEVICE_CONFIG_2_I2C_GLITCH_FILTER_Off;
    TMAG5x73writeToSingleRegister(DEVICE_CONFIG_2_ADDRESS, input);
}
//****************************************************************************
//****************************************************************************
//
// Change Device Operation Mode
//
//****************************************************************************
//****************************************************************************

//****************************************************************************
//! Enter Stand-by Mode
//****************************************************************************
void TMAG5x73enterStandbyMode()
{
    uint8_t input;
    // Set OPERATING_MODE to Standby Mode (0h)
    input = TMAG5x73readSingleRegister(DEVICE_CONFIG_2_ADDRESS, Trigger_Conversion_Enabled);
    input = (input & ~(DEVICE_CONFIG_2_OPERATING_MODE_MASK)) | DEVICE_CONFIG_2_OPERATING_MODE_Standby;
    TMAG5x73writeToSingleRegister(DEVICE_CONFIG_2_ADDRESS, input);

#ifdef  MAX_DELAYS_IN_OPMODE_CHANGES
    delay_us(50); // max expected delay as given by t_start_sleep (datasheet pg. 8 (TMAG5273) / pg. 9 TMAG5173-Q1)
#endif
}



//****************************************************************************
//! Enter Continuous Measure Mode (continuous conversion)
//****************************************************************************
void TMAG5x73enterContinuousMeasureMode()
{
    uint8_t input;
    // Set OPERATING_MODE to Continuous Measure Mode (2h)
    input = TMAG5x73readSingleRegister(DEVICE_CONFIG_2_ADDRESS, Trigger_Conversion_Enabled);
    input = (input & ~(DEVICE_CONFIG_2_OPERATING_MODE_MASK)) | DEVICE_CONFIG_2_OPERATING_MODE_ContinuousMeasure;
    TMAG5x73writeToSingleRegister(DEVICE_CONFIG_2_ADDRESS, input);

#ifdef  MAX_DELAYS_IN_OPMODE_CHANGES
    delay_us(120); // max expected delay as given by t_start_sleep + t_start_measure (datasheet: TMAG5273 pg. 8 & pg.9, TMAG5173-Q1 pg.9)
#endif
}



//****************************************************************************
//! Enter Sleep Mode
//!
//! NOTE: This function MUST be the last I2C communication with the device or
//!       this device will exit sleep mode and go into Stand-by Mode
//****************************************************************************
void TMAG5x73enterSleepMode()
{
    uint8_t input;
    // Set OPERATING_MODE to Sleep Mode (1h)
    input = TMAG5x73readSingleRegister(DEVICE_CONFIG_2_ADDRESS, Trigger_Conversion_Enabled); // NOTE: this line may need to be changed if using a 1-byte I2C read
    input = (input & ~(DEVICE_CONFIG_2_OPERATING_MODE_MASK)) | DEVICE_CONFIG_2_OPERATING_MODE_Sleep;
    TMAG5x73writeToSingleRegister(DEVICE_CONFIG_2_ADDRESS, input);

#ifdef  MAX_DELAYS_IN_OPMODE_CHANGES
    delay_us(20); // max expected delay as given by t_go_sleep (datasheet pg. 9)
#endif
}



//****************************************************************************
//! Enter Wake and Sleep Mode
//! Does not change the SLEEPTIME bits
//!
//! NOTE: This function MUST be the last I2C communication with the device or
//!       this device will exit wake & sleep mode and go into Stand-by Mode
//****************************************************************************
void TMAG5x73enterWakeUpAndSleepMode()
{
    uint8_t input;
    // Set OPERATING_MODE to Wake-Up and Sleep Mode (3h)
    input = TMAG5x73readSingleRegister(DEVICE_CONFIG_2_ADDRESS, Trigger_Conversion_Enabled); // NOTE: this line may need to be changed if using a 1-byte I2C read
    input = (input & ~(DEVICE_CONFIG_2_OPERATING_MODE_MASK)) | DEVICE_CONFIG_2_OPERATING_MODE_WakeupandSleep;
    TMAG5x73writeToSingleRegister(DEVICE_CONFIG_2_ADDRESS, input);

#ifdef  MAX_DELAYS_IN_OPMODE_CHANGES
    delay_us(20); // max expected delay as given by t_go_sleep (datasheet pg. 9)
#endif
}



//****************************************************************************
//! Set SLEEPTIME
//! Takes in input from 0x00 to 0x0C to determine SLEEPTIME field according to
//! datasheet (pg. 28 (TMAG5273) / pg. 31 (TMAG5173-Q1))
//! Does not change OPERATING_MODE and will not begin Wake and Sleep Mode
//!
//! SLEEPTIME determines the amount of time spend in low power mode between device conversions
//! while in Wake Up and Sleep mode (OPERATING_MODE = 4h)
//!                          |
//!          SLEEPTIME bits  |  time between convs.
//!         _________________|_______________________
//!               0x00                   1ms
//!               0x01                   5ms
//!               0x02                  10ms
//!               0x03                  15ms
//!               0x04                  20ms
//!               0x05                  30ms
//!               0x06                  50ms
//!               0x07                 100ms
//!               0x08                 500ms
//!               0x09                1000ms
//!               0x0A                2000ms
//!               0x0B                5000ms
//!               0x0C               20000ms
//****************************************************************************
void TMAG5x73setSLEEPTIME(uint8_t sleeptime)
{
    // Checks that input is valid
    if (!(sleeptime <= 0x0C)) return;

    uint8_t input;

    // Set SLEEPTIME to sleeptime (corresponding time values are on datasheet pg. 28 (TMAG5273) / pg. 31 (TMAG5173-Q1))
    input = TMAG5x73readSingleRegister(SENSOR_CONFIG_1_ADDRESS, Trigger_Conversion_Enabled);  // NOTE: this line may need to be changed if using a 1-byte I2C read
    input = (input & ~(SENSOR_CONFIG_1_SLEEPTIME_MASK)) | (sleeptime);
    TMAG5x73writeToSingleRegister(SENSOR_CONFIG_1_ADDRESS, input);
}



//****************************************************************************
//! Wake Up and Sleep Mode with SLEEPTIME set
//! Takes in input from 0x00 to 0x0F to set the SLEEPTIME field according to datasheet
//!
//! NOTE: See TMAG5x73setSLEEPTIME() function to see possible sleeptime values
//!
//! NOTE: This function MUST be the last I2C communication with the device or
//!       this device will exit wake & sleep mode and go into Stand-by Mode
//****************************************************************************
void TMAG5x73setWakeUpAndSleepMode(uint8_t sleeptime)
{
    // Checks that input is valid
    if (!(sleeptime <= 0x0C)) return;

    TMAG5x73setSLEEPTIME(sleeptime);
    TMAG5x73enterWakeUpAndSleepMode();
}



//****************************************************************************
//! Exit Sleep Mode
//!
//! Exits Sleep Mode by setting the OPERATING_MODE field to Standby Mode.
//****************************************************************************
void TMAG5x73exitSleepMode()
{
    uint8_t input;
    // Set OPERATING_MODE to Standby Mode (0h)
    input = TMAG5x73readSingleRegister(DEVICE_CONFIG_2_ADDRESS, Trigger_Conversion_Enabled);
    input = (input & ~(DEVICE_CONFIG_2_OPERATING_MODE_MASK)) | DEVICE_CONFIG_2_OPERATING_MODE_Standby;
    TMAG5x73writeToSingleRegister(DEVICE_CONFIG_2_ADDRESS, input);

#ifdef  MAX_DELAYS_IN_OPMODE_CHANGES
    delay_us(50); // max expected delay as given by t_start_sleep (datasheet pg. 8 (TMAG5273) / pg. 9 TMAG5173-Q1)
#endif
}



//****************************************************************************
//! Exit Wake Up and Sleep Mode
//!
//! Exits Wake Up and Sleep Mode by setting the OPERATING_MODE field to Standby Mode.
//****************************************************************************
void TMAG5x73exitWakeAndSleepMode()
{
    uint8_t input;
    // Set OPERATING_MODE to Standby Mode (0h)
    input = TMAG5x73readSingleRegister(DEVICE_CONFIG_2_ADDRESS, Trigger_Conversion_Enabled);
    input = (input & ~(DEVICE_CONFIG_2_OPERATING_MODE_MASK)) | DEVICE_CONFIG_2_OPERATING_MODE_Standby;
    TMAG5x73writeToSingleRegister(DEVICE_CONFIG_2_ADDRESS, input);

#ifdef  MAX_DELAYS_IN_OPMODE_CHANGES
    delay_us(50); // max expected delay as given by t_start_sleep (datasheet pg. 8 (TMAG5273) / pg. 9 TMAG5173-Q1)
#endif
}



//****************************************************************************
//****************************************************************************
//
// Functions to Configure Trigger Conversion Settings
//
//****************************************************************************
//****************************************************************************

//****************************************************************************
//! I2C to Trigger Conversion
//!
//! Configures the conversion trigger to be activated on a I2C read or write command
//****************************************************************************
void TMAG5x73i2cTriggersConversion()
{
    uint8_t input;
    // SET TRIGGER_MODE to 'Conversion Start at I2C Command Bits' (0h)
    input = TMAG5x73readSingleRegister(DEVICE_CONFIG_2_ADDRESS, Trigger_Conversion_Enabled);
    input &= ~(DEVICE_CONFIG_2_TRIGGER_MODE_MASK);
    input |= DEVICE_CONFIG_2_TRIGGER_MODE_I2CCommandBits;
    TMAG5x73writeToSingleRegister(DEVICE_CONFIG_2_ADDRESS, input);
}



//****************************************************************************
//! INT to Trigger Conversion
//!
//! Configures the conversion trigger to be activated on a LOW pulse to INT.
//!
//! i2c_busy  - input determining if interrupt should occur when I2C bus is busy
//!             0x00 = Interrupt through INT
//!             0x01 = Interrupt through INT, except when busy
//!
//****************************************************************************
void TMAG5x73intTriggersConversion(uint8_t i2c_busy)
{
    // Checks if input is valid
    if ((i2c_busy > 0x01)) return;

    uint8_t input;
    // SET TRIGGER_MODE to start conversion at INT pin
    input = TMAG5x73readSingleRegister(DEVICE_CONFIG_2_ADDRESS, Trigger_Conversion_Enabled);
    input &= ~(DEVICE_CONFIG_2_TRIGGER_MODE_MASK);
    input |= DEVICE_CONFIG_2_TRIGGER_MODE_TriggerSignalatINTPin;
    TMAG5x73writeToSingleRegister(DEVICE_CONFIG_2_ADDRESS, input);

    // SET INT_MODE to i2c_busy
    input = TMAG5x73readSingleRegister(INT_CONFIG_1_ADDRESS, Trigger_Conversion_Enabled);
    input &= ~(INT_CONFIG_1_INT_MODE_MASK);
    input |= INT_CONFIG_1_INT_MODE_INT + ((i2c_busy) << 2);

    TMAG5x73writeToSingleRegister(INT_CONFIG_1_ADDRESS, input);
}




//****************************************************************************
//****************************************************************************
//
// Threshold Detection + INT output Settings
//
//****************************************************************************
//****************************************************************************


//****************************************************************************
//! Set INT Pin to Either Be Latched or Pulsed
//!
//! Configures the device so that when the device outputs a LOW on the INT pin
//! it will either stay latched until cleared by a primary or interrupt pulse
//! for 10 us output LOW on the INT pin.
//!
//! i2c_busy  - input determining if interrupt should occur when I2C bus is busy
//!             0x00 = Interrupt through INT
//!             0x01 = Interrupt through INT, except when busy
//!
//! int_state - input determining if interrupt should be latched or pulsed
//!             0x00 = interrupt latched until cleared by primary addressing device
//!             0x01 = interrupt pulse for 10us
//****************************************************************************
void TMAG5x73intPinLatchedPulsed(uint8_t i2c_busy, uint8_t int_state)
{
    if ((int_state > 0x01)) return;

    uint8_t input;
    input = TMAG5x73readSingleRegister(INT_CONFIG_1_ADDRESS, Trigger_Conversion_Enabled);
    input &= ~(INT_CONFIG_1_INT_MODE_MASK);

    // SET INT_MODE to i2c_busy
    input |= INT_CONFIG_1_INT_MODE_INT + ((i2c_busy) << 2);

    // SET INT_STATE to be latched/pulsed
    input |= INT_CONFIG_1_INT_STATE_UntilClear + (int_state << 5);
    TMAG5x73writeToSingleRegister(INT_CONFIG_1_ADDRESS, input);
}



//****************************************************************************
//! Enable INT to Indicate Conversion Complete
//!
//! Configures the device so when its magnetic measurements are complete, the device will
//! output LOW on the INT pin.
//!
//! i2c_busy  - input determining if interrupt should occur when I2C bus is busy
//!             0x00 = Interrupt through INT
//!             0x01 = Interrupt through INT, except when busy
//!
//! NOTE: This configures INT as an output pin, the respective GPIO pin it is
//!       connected to will have to be set as an input as well. Please ensure none
//!       of the input functions of INT are being used as well
//****************************************************************************
void TMAG5x73intIndicatesConversionEnable(uint8_t i2c_busy)
{
    // Checks if inputs are valid
    if ((i2c_busy > 0x01)) return;

    uint8_t input;
    // SET INT_MODE to i2c_busy
    input = TMAG5x73readSingleRegister(INT_CONFIG_1_ADDRESS, Trigger_Conversion_Enabled);
    input &= ~(INT_CONFIG_1_INT_MODE_MASK);
    input |= INT_CONFIG_1_INT_MODE_INT + ((i2c_busy) << 2);

    // SET RSLT_INT to enable interrupt response to indicate conversion completion (1h)
    input |= INT_CONFIG_1_RSLT_INT_Enabled;

    // SET MASK_INTB to enable interrupt pin (0h)
    input |= INT_CONFIG_1_MASK_INTB_Enabled;
    TMAG5x73writeToSingleRegister(INT_CONFIG_1_ADDRESS, input);
}



//****************************************************************************
//! Enable SCL to Indicate Conversion Complete
//!
//! Configures the device so when its magnetic measurements are complete, the device will
//! output LOW on the SCL pin.
//!
//! i2c_busy - input determining if interrupt should occur when I2C bus is busy
//!            0x00 = Interrupt through SCL
//!            0x01 = Interrupt through SCL, except when busy
//!
//! NOTE: This configures SCL as an output pin, the respective GPIO pin it is
//!       connected to will have to be set as an input as well. Please ensure none
//!       of the input functions of SCL are being used as well
//****************************************************************************
void TMAG5x73sclIndicatesConversionEnable(uint8_t i2c_busy)
{
    // Checks if input is valid
    if ((i2c_busy > 0x01)) return;

    uint8_t input;
    // SET INT_MODE to i2c_busy
    input = TMAG5x73readSingleRegister(INT_CONFIG_1_ADDRESS, Trigger_Conversion_Enabled);
    input &= ~(INT_CONFIG_1_INT_MODE_MASK);
    input |= INT_CONFIG_1_INT_MODE_SCL + ((i2c_busy) << 2);

    // SET RSLT_INT to enable interrupt response to indicate conversion completion (1h)
    // (Register already grabbed so no new READ command is needed)
    input |= INT_CONFIG_1_RSLT_INT_Enabled;
    TMAG5x73writeToSingleRegister(INT_CONFIG_1_ADDRESS, input);
}



//****************************************************************************
//! Disable interrupt to Indicate Conversion Complete
//!
//! Turns off the INT pin assertion when device measurements are complete.
//****************************************************************************
void TMAG5x73interruptIndicatesConversionDisable()
{
    uint8_t input;
    // SET RSLT_INT INT is not asserted to indicate conversion completion (0h)
    input = TMAG5x73readSingleRegister(INT_CONFIG_1_ADDRESS, Trigger_Conversion_Enabled);
    input &= ~(INT_CONFIG_1_RSLT_INT_MASK);
    input |= INT_CONFIG_1_RSLT_INT_Disabled;
    TMAG5x73writeToSingleRegister(INT_CONFIG_1_ADDRESS, input);
}



//****************************************************************************
//! Setup Magnetic Switch Mode (Only Available for TMAG5173)
//!
//! Enables Magnetic Switch Mode for the INT pin and the respective high/low
//! threshold triggers. The thresholds are not set using this function, use
//! TMAG5x73magThreshSet to change the high/low thresholds for each measurement.
//!
//! switch_type - input determining the type of switch to enable
//!               0x00 = Unipolar | 0x01 = Omnipolar
//!
//! NOTE: This configures INT as an output pin, the respective GPIO pin it is
//!       connected to will have to be set as an input as well. Please ensure none
//!       of the input functions of INT are being used as well (such as the INT trigger)
//! NOTE: This mode overrides any interrupt function. Select THR_HYST > 001b for this mode.
//****************************************************************************
void TMAG5173setMagSwitch(uint8_t switch_type)
{
    // Check that inputs are valid
    if(switch_type > 0x01) return;

    uint8_t input;

    // Set INT_MODE field in INT_CONFIG_1 to switch_type
    input = TMAG5x73readSingleRegister(INT_CONFIG_1_ADDRESS, Trigger_Conversion_Enabled);
    input &= ~(INT_CONFIG_1_INT_MODE_MASK);
    input |= INT_CONFIG_1_INT_MODE_UnipolarSwitchMode + (switch_type << 2);
    TMAG5x73writeToSingleRegister(INT_CONFIG_1_ADDRESS, input);

}



//****************************************************************************
//! Disable Interrupts
//!
//! Changes the INT_MODE back to No interrupts
//****************************************************************************
void TMAG5x73disableInterruptModes()
{
    uint8_t input;
    // Set INT_MODE field to disable interrupts (0h)
    input = TMAG5x73readSingleRegister(INT_CONFIG_1_ADDRESS, Trigger_Conversion_Enabled);
    input &= ~(INT_CONFIG_1_INT_MODE_MASK);
    input |= INT_CONFIG_1_INT_MODE_Disabled;
    TMAG5x73writeToSingleRegister(INT_CONFIG_1_ADDRESS, input);
}



//****************************************************************************
//! Disable INT pin
//****************************************************************************
void TMAG5x73disableMaskINTpin()
{
    uint8_t input;
    // Set MASK_INTB field to disable interrupts (1h)
    input = TMAG5x73readSingleRegister(INT_CONFIG_1_ADDRESS, Trigger_Conversion_Enabled);
    input &= ~(INT_CONFIG_1_MASK_INTB_MASK);
    input |= INT_CONFIG_1_MASK_INTB_Disabled;
    TMAG5x73writeToSingleRegister(INT_CONFIG_1_ADDRESS, input);
}



//****************************************************************************
//! Updates the Threshold Settings
//!
//! Enables an interrupt response on a predefined threshold cross determined
//! by the threshold set by the user
//!
//! thr_direction                  - which direction of the threshold to check
//!                                  NOTE: this gets ignored if THR_HYST > 0x01
//!                                  0x00 = above the threshold | 0x01 = below the threshold
//!
//! thr_crossings                  - number of threshold crossings before interrupt is asserted
//!                                  0x00 = 1 threshold crossing | 0x01 = 4 threshold crossings
//!
//! thr_hyst                       - selects thresholds for the interrupt function
//!                                  0x00 = 2's complement value and creates magnetic threshold
//!                                  0x01 = 7 LSB bits and creates two opposite magnetic thresholds
//!                                         (one north and another south) of equal magnitude.
//!                                  0x02 = 8 LSB of threshold at 12 bit resolution   (TMAG5173 Only)
//!                                  0x03 = 16 LSB of threshold at 12 bit resolution  (TMAG5173 Only)
//!                                  0x04 = 32 LSB of threshold at 12 bit resolution  (TMAG5173 Only)
//!                                  0x05 = 64 LSB of threshold at 12 bit resolution  (TMAG5173 Only)
//!                                  0x06 = 128 LSB of threshold at 12 bit resolution (TMAG5173 Only)
//!                                  0x07 = 256 LSB of threshold at 12 bit resolution (TMAG5173 Only)
//****************************************************************************
void TMAG5x73thresholdSettings(uint8_t thr_direction, uint8_t thr_crossings, uint8_t thr_hyst)
{
    // Check that inputs are valid
    if(!(thr_direction <= 0x01)) return;
    if(!(thr_crossings <= 0x01)) return;
    if(!(thr_hyst <= 0x07)) return;

    uint8_t input;

    // Set THRSLD_INT field to Interrupt is asserted when a threshold is crossed (1h)
    input = TMAG5x73readSingleRegister(INT_CONFIG_1_ADDRESS, Trigger_Conversion_Enabled);
    input &= ~(INT_CONFIG_1_THRSLD_INT_MASK);
    input |= INT_CONFIG_1_THRSLD_INT_Enabled;
    TMAG5x73writeToSingleRegister(INT_CONFIG_1_ADDRESS, input);

    // Sets THRX_COUNTs to thr_direction and sets MAG_THR_DIR field to thr_crossings
    input = TMAG5x73readSingleRegister(SENSOR_CONFIG_2_ADDRESS, Trigger_Conversion_Enabled);
    input &= ~(SENSOR_CONFIG_2_THRX_COUNT_MASK);
    input &= ~(SENSOR_CONFIG_2_MAG_THR_DIR_MASK);
    input |= SENSOR_CONFIG_2_THRX_COUNT_One + (thr_crossings << 6);
    input |= SENSOR_CONFIG_2_MAG_THR_DIR_AboveThreshold + (thr_direction << 5);
    TMAG5x73writeToSingleRegister(SENSOR_CONFIG_2_ADDRESS, input);

    // Set THR_HYST field to thr_hyst
    input = TMAG5x73readSingleRegister(DEVICE_CONFIG_2_ADDRESS, Trigger_Conversion_Enabled);
    input &= ~(DEVICE_CONFIG_2_THR_HYST_MASK);
    input |= DEVICE_CONFIG_2_THR_HYST_2sComplement + (thr_hyst << 5);
    TMAG5x73writeToSingleRegister(DEVICE_CONFIG_2_ADDRESS, input);
}



//****************************************************************************
//! Setup Magnetic Thresholds
//!
//! Enables an interrupt response on a predefined threshold cross determined
//! by the threshold set by the user
//!
//! set_threshold                  - which threshold is being set (one at a time):
//!                                  0x00 = X  |  0x01 = Y  |  0x02 = Z  |  0x03 = T
//!
//! thr (2's Complement for X/Y/Z) - 2's Complement 8-bit value for the X/Y/Z threshold
//!                                  (check datasheet for conversion eqn - pg. 29 (TMAG5273)/
//!                                  pgs. 33-36 (TMAG5173-Q1))
//!
//! NOTE: If setting the threshold for temperature, please note that valid temperature
//!       ranges are between -41C to 170C with the threshold codes for -41C = 0x1A
//!       and 170C = 0x34. Resolution is 8 degrees C/LSB
//! NOTE: Signed ints were used in the function call to make input easier.
//!       Unsigned ints must be used to combine into one data line
//****************************************************************************
void TMAG5x73magThreshSet(uint8_t set_threshold, int8_t thr)
{
    // Check that inputs are valid
    if(!(set_threshold <= 0x03)) return;
    if((set_threshold == 0x03) && (thr < 0x1A || thr > 0x34)) return;

    uint8_t input;

    // Get associated register address (register order is XYZT from 0x04 to 0x07)
    // The set_threshold value can be added to X address to get the desired one
    uint8_t reg_address = X_THR_CONFIG_ADDRESS + set_threshold;

    // If the threshold being set is for temperature, then thr needs to be
    // shifted one bit to the left as Bit 0 in T_CONFIG is used to determine
    // if the temperature channel should be enabled
    if (reg_address == T_CONFIG_ADDRESS)
        input = ((uint8_t) thr) << 1;
    else
        input = ((uint8_t) thr);

    TMAG5x73writeToSingleRegister(reg_address, input);
}



//****************************************************************************
//! Disable Magnetic Thresholds
//!
//! Disables interrupt assertion when threshold is crossed and sets the X/Y/Z/T
//! threshold registers (0x04 to 0x07) so that there is no threshold comparison
//****************************************************************************
void TMAG5x73disableMagThresholds()
{
    uint8_t input;
    // Set THRSLD_INT field to Interrupt is not asserted when a threshold is crossed (0h)
    input = TMAG5x73readSingleRegister(INT_CONFIG_1_ADDRESS, Trigger_Conversion_Enabled);
    input &= ~(INT_CONFIG_1_THRSLD_INT_MASK);
    input |= INT_CONFIG_1_THRSLD_INT_Disabled;
    TMAG5x73writeToSingleRegister(INT_CONFIG_1_ADDRESS, input);

    uint8_t reg_address;
    int i;
    input = 0x00;

    for (i = 0; i < 3; i++)
    {
        reg_address = X_THR_CONFIG_ADDRESS + i;
        TMAG5x73writeToSingleRegister(reg_address, input);
    }

    input = TMAG5x73readSingleRegister(T_CONFIG_ADDRESS, Trigger_Conversion_Enabled);
    input &= ~(T_CONFIG_T_THR_CONFIG_MASK);
    TMAG5x73writeToSingleRegister(T_CONFIG_ADDRESS, input);
}



//****************************************************************************
//****************************************************************************
//
// Measurement Configuration Functions
//
//****************************************************************************
//****************************************************************************

//****************************************************************************
//! Enable Magnetic Axes for Measurement (also can turn all channels off)
//!
//! Takes in a 4-bit value for the MAG_CH_EN field
//! mag_ch_en_bits must not be greater than 0x0F
//!
//! NOTE: Only the TMAG5173-Q1 has access to the features described when
//!       mag_ch_en_bits > 0x0B. These bits on the TMAG5273 are reserved
//!       and unavailable.
//!
//!                      | enabled channels ||                  |          enabled channels
//!      mag_ch_en_bits  | + sampling order ||  mag_ch_en_bits  |          + sampling order
//!     _________________|__________________||__________________|____________________________________
//!           0x00               none       ||       0x08                        XYX
//!           0x01                X         ||       0x09                        YXY
//!           0x02                Y         ||       0x0A                        YZY
//!           0x03               XY         ||       0x0B                        XZX
//!           0x04                Z         ||       0x0C           XYZ Positive Diagnostic Offset
//!           0x05               ZX         ||       0x0D           XYZ Negative Diagnostic Offset
//!           0x06               YZ         ||       0x0E             Hall Resistance & ADC Check
//!           0x07               XYZ        ||       0x0F               Hall Offset & AFE Check
//!
//! Definitions for descriptive inputs to this function are provided in the header file.
//****************************************************************************
void TMAG5x73enableMagChannels(uint8_t mag_ch_en_bits)
{
    // Check that input is valid
    if (!(mag_ch_en_bits <= 0x0F)) return;

    uint8_t input;
    // Set MAG_CH_EN to mag_ch_en_bits
    input = TMAG5x73readSingleRegister(SENSOR_CONFIG_1_ADDRESS, Trigger_Conversion_Enabled);
    input = (input & ~(SENSOR_CONFIG_1_MAG_CH_EN_MASK)) | (mag_ch_en_bits << 4);
    TMAG5x73writeToSingleRegister(SENSOR_CONFIG_1_ADDRESS, input);
}



//****************************************************************************
//! Enable Angle Measurement (also can turn off angle measurement)
//!
//! Takes in a 2-bit value for the ANGLE_EN field to determine which two
//! axes to measure the angle off of for the on-board CORDIC function in the device.
//!
//! Based on the ANGLE_EN setting, the angle will be calculated using the first axis
//! as the "horizontal" axis (positive side of axis is 0 degrees) and the second as
//! the "vertical" axis (positive side of axis is 90 degrees)
//!
//! angle_en_bits can be set to 0x00 to 0x03 to configure these settings for ANGLE_EN:
//!          ANGLE_EN value | horizontal axis | vertical axis
//!               0x00              none            none  --  (angle measurement disabled)
//!               0x01               X               Y
//!               0x02               Y               Z
//!               0x03               X               Z
//****************************************************************************
void TMAG5x73enableAngleMeasurement(uint8_t angle_en_bits)
{
    // Check that input is valid
    if (!(angle_en_bits <= 0x03)) return;

    uint8_t input;
    // Set ANGLE_EN to angle_en_bits
    input = TMAG5x73readSingleRegister(SENSOR_CONFIG_2_ADDRESS, Trigger_Conversion_Enabled);
    input = (input & ~(SENSOR_CONFIG_2_ANGLE_EN_MASK)) | (angle_en_bits << 2);
    TMAG5x73writeToSingleRegister(SENSOR_CONFIG_2_ADDRESS, input);
}



//****************************************************************************
//! Enable Temperature Measurement
//!
//! Begins Temperature Measurements by changing the T_CH_EN field in the
//! DEVICE_CONFIG to 1b.
//****************************************************************************
void TMAG5x73enableTemperatureMeasurement()
{
    // Set T_CH_EN to Temp channel enabled (1h)
    uint8_t input = TMAG5x73readSingleRegister(T_CONFIG_ADDRESS, Trigger_Conversion_Enabled);
    input &= ~(T_CONFIG_T_CH_EN_MASK);
    input |= T_CONFIG_T_CH_EN_Enabled;
    TMAG5x73writeToSingleRegister(T_CONFIG_ADDRESS, input);
}

//****************************************************************************
//! Disable Temperature Measurement
//!
//! Ends Temperature Measurements by changing the T_CH_EN field in the
//! DEVICE_CONFIG to 0b.
//****************************************************************************
void TMAG5x73disableTemperatureMeasurement()
{
    // Set T_CH_EN to Temp channel disabled (0h)
    uint8_t input = TMAG5x73readSingleRegister(T_CONFIG_ADDRESS, Trigger_Conversion_Enabled);
    input &= ~(T_CONFIG_T_CH_EN_MASK);
    input |= T_CONFIG_T_CH_EN_Disabled;
    TMAG5x73writeToSingleRegister(T_CONFIG_ADDRESS, input);
}



//****************************************************************************
//! Sets Temperature Coefficient of the Magnet
//!
//! temp_coefficient - Temperature coefficient of the magnet
//!                    0x00 = 0% (No temperature coefficient)
//!                    0x01 = 0.12%/deg C (NdBFe)
//!                    0x02 = 0.03%/deg C (SmCo)     (TMAG5173 Only)
//!                    0x03 = 0.2%/deg C (Ceramic)
//****************************************************************************
void TMAG5x73setTempCoefficient(uint8_t temp_coefficient)
{
    // Check that input is valid
    if (!(temp_coefficient <= 0x03)) return;

    // Set MAG_TEMPCO to temp_coefficient
    uint8_t input = TMAG5x73readSingleRegister(DEVICE_CONFIG_1_ADDRESS, Trigger_Conversion_Enabled);
    input &= ~(DEVICE_CONFIG_1_MAG_TEMPCO_MASK);
    input |= DEVICE_CONFIG_1_MAG_TEMPCO_None + (temp_coefficient << 5);
    TMAG5x73writeToSingleRegister(DEVICE_CONFIG_1_ADDRESS, input);
}



//****************************************************************************
//! Set Sampling Rate (configure the amount of additional samples to reduce noise/increase resolution)
//!
//! CONV_AVG_bits - what value to set for the CONV_AVG value (from datasheet pg. 26 (TMAG5273) / pg. 29 (TMAG5173-Q1)):
//!                      | num. samples | 3-axes speed | 1-axis speed |
//!               0x00 -         1x         10.0Ksps         20Ksps
//!               0x01 -         2x          5.7Ksps       13.3Ksps
//!               0x02 -         4x          3.1Ksps        8.0Ksps
//!               0x03 -         8x          1.6Ksps        4.4Ksps
//!               0x04 -        16x          0.8Ksps        2.4Ksps
//!               0x05 -        32x          0.4Ksps        1.2Ksps
//****************************************************************************
void TMAG5x73setSampleRate(uint8_t CONV_AVG_bits)
{
    // Check that input is valid
    if (!(CONV_AVG_bits <= 0x05)) return;

    uint8_t input;
    // Set CONV_AVG to CONV_AVG_bits
    input = TMAG5x73readSingleRegister(DEVICE_CONFIG_1_ADDRESS, Trigger_Conversion_Enabled);
    input = (input & ~(DEVICE_CONFIG_1_CONV_AVG_MASK)) | (CONV_AVG_bits << 2);
    TMAG5x73writeToSingleRegister(DEVICE_CONFIG_1_ADDRESS, input);
}



//****************************************************************************
//! Set Ranges for X, Y, and Z axes
//!
//! Sets the X, Y, and Z_RANGE fields in the SENSOR_CONFIG_2 register to the bits
//! determined by the function inputs.
//!
//! x_y_range_bits - bits for X_Y_RANGE field (must be no greater than 0x01)
//! z_range_bits - bits for Z_RANGE field (must be no greater than 0x01)
//!
//! According to the TMAG5x73 version used, the mT range for the bits are as follows:
//!
//!                 *_range_bits  |   TMAG5x73A1   |   TMAG5x73A2
//!                    input      | mT range value | mT range value
//!              _________________|________________|________________
//!                    0x00       |      40 mT     |     133 mT
//!                    0x01       |      80 mT     |     266 mT
//****************************************************************************
void TMAG5x73setRanges(uint8_t x_y_range_bits, uint8_t z_range_bits)
{
    // Check that inputs are valid
    if (x_y_range_bits > 0x01 || z_range_bits > 0x01) return;

    // Set X_Y_RANGE to x_y_range_bits and Z_RANGE to z_range_bits
    uint8_t input = TMAG5x73readSingleRegister(SENSOR_CONFIG_2_ADDRESS, Trigger_Conversion_Enabled) & ~(SENSOR_CONFIG_2_FULL_RANGE_MASK);
    input |= (x_y_range_bits << 1) | z_range_bits;
    TMAG5x73writeToSingleRegister(SENSOR_CONFIG_2_ADDRESS, input);
}



//****************************************************************************
//! Set the Low Mode to Low Active Current or Low Noise
//!
//! low_mode - selects a mode between low active current and low noise
//!            0x00 = Low active current mode
//!            0x01 = Low noise mode
//****************************************************************************
void TMAG5x73selectLowCurrentNoiseMode(uint8_t low_mode)
{
    // Check that input is valid
    if (!(low_mode <= 0x01)) return;

    uint8_t input;
    // Set LP_LN to low_mode
    input = TMAG5x73readSingleRegister(DEVICE_CONFIG_2_ADDRESS, Trigger_Conversion_Enabled);
    input &= ~(DEVICE_CONFIG_2_LP_LN_MASK);
    input |= DEVICE_CONFIG_2_LP_LN_LowActiveCurrent + (low_mode << 4);
    TMAG5x73writeToSingleRegister(DEVICE_CONFIG_2_ADDRESS, input);
}



//****************************************************************************
//****************************************************************************
//
// Get Results/Measurement Functions (Standard 3-byte I2C Read Only)
//
//****************************************************************************
//****************************************************************************

//****************************************************************************
//! Get and return the *_*SB_RESULT register for an axis/measurement
//!
//! These functions explicitly return the unsigned 8-bit register of their respective
//! measurement address.
//****************************************************************************
uint8_t TMAG5x73getXMSBresult() { return TMAG5x73readSingleRegister(X_MSB_RESULT_ADDRESS, Trigger_Conversion_Enabled); }
uint8_t TMAG5x73getXLSBresult() { return TMAG5x73readSingleRegister(X_LSB_RESULT_ADDRESS, Trigger_Conversion_Enabled); }
uint8_t TMAG5x73getYMSBresult() { return TMAG5x73readSingleRegister(Y_MSB_RESULT_ADDRESS, Trigger_Conversion_Enabled); }
uint8_t TMAG5x73getYLSBresult() { return TMAG5x73readSingleRegister(Y_LSB_RESULT_ADDRESS, Trigger_Conversion_Enabled); }
uint8_t TMAG5x73getZMSBresult() { return TMAG5x73readSingleRegister(Z_MSB_RESULT_ADDRESS, Trigger_Conversion_Enabled); }
uint8_t TMAG5x73getZLSBresult() { return TMAG5x73readSingleRegister(Z_LSB_RESULT_ADDRESS, Trigger_Conversion_Enabled); }
uint8_t TMAG5x73getTempMSBresult() { return TMAG5x73readSingleRegister(T_MSB_RESULT_ADDRESS, Trigger_Conversion_Enabled); }
uint8_t TMAG5x73getTempLSBresult() { return TMAG5x73readSingleRegister(T_LSB_RESULT_ADDRESS, Trigger_Conversion_Enabled); }
uint8_t TMAG5x73getAngleMSBresult() { return TMAG5x73readSingleRegister(ANGLE_RESULT_MSB_ADDRESS, Trigger_Conversion_Enabled); }
uint8_t TMAG5x73getAngleLSBresult() { return TMAG5x73readSingleRegister(ANGLE_RESULT_LSB_ADDRESS, Trigger_Conversion_Enabled); }
uint8_t TMAG5x73getMAGresult() { return TMAG5x73readSingleRegister(MAGNITUDE_RESULT_ADDRESS, Trigger_Conversion_Enabled); }
// NOTE: These functions returned the unsigned integer corresponding to the register value
//       for easier bit operations. To convert the unsigned using the equations used in the
//       example code, it must be casted to an signed integer.



//****************************************************************************
//! Get Magnetic Results Registers
//!
//! Takes in a size 3 array of int16_t values and assigns it the the
//! three *_*SB_RESULT registers. (order XYZ)
//!
//! INPUT ARRAY MUST BE OF LENGTH 3
//!
//! NOTE: The uint16_t variables returned by the get*result functions are casted
//! into int16_t variables for use with the other provided functions that take
//! signed integers.
//****************************************************************************
void TMAG5x73getMagResultsRegisters(int16_t meas_arr[])
{
    meas_arr[0] = (((int16_t) TMAG5x73getXMSBresult()) << 8) | ((int16_t) TMAG5x73getXLSBresult());
    meas_arr[1] = (((int16_t) TMAG5x73getYMSBresult()) << 8) | ((int16_t) TMAG5x73getYLSBresult());
    meas_arr[2] = (((int16_t) TMAG5x73getZMSBresult()) << 8) | ((int16_t) TMAG5x73getZLSBresult());
}


//****************************************************************************
//! Get X-axis Magnetic Flux Measurement in mT
//!
//! Returns a float containing the mT magnetic flux measurement converted from the
//! X_MSB_RESULT and X_LSB_RESULT registers.
//****************************************************************************
float TMAG5x73getMeasurementX()
{

    uint16_t range = TMAG5x73getXYrange();
    int16_t data = ((int16_t) TMAG5x73getXMSBresult() << 8) | ((int16_t) TMAG5x73getXLSBresult()); // separate variable used to cast to a signed int
                                                                                                   // for the float cast to work correctly

    return (((float) data) / 32768) * range;
}



//****************************************************************************
//! Get Y-axis Magnetic Flux Measurement in mT
//!
//! Returns a float containing the mT magnetic flux measurement converted from the
//! Y_MSB_RESULT and Y_LSB_RESULT registers.
//****************************************************************************
float TMAG5x73getMeasurementY()
{

    uint16_t range = TMAG5x73getXYrange();
    int16_t data = ((int16_t) TMAG5x73getYMSBresult() << 8) | ((int16_t) TMAG5x73getYLSBresult()); // separate variable used to cast to a signed int
                                                                                                   // for the float cast to work correctly

    return (((float) data) / 32768) * range;
}



//****************************************************************************
//! Get Z-axis Magnetic Flux Measurement in mT
//!
//! Returns a float containing the mT magnetic flux measurement converted from the
//! Z_MSB_RESULT and Z_LSB_RESULT registers.
//****************************************************************************
float TMAG5x73getMeasurementZ()
{

    uint16_t range = TMAG5x73getZrange();
    int16_t data = ((int16_t) TMAG5x73getZMSBresult() << 8) | ((int16_t) TMAG5x73getZLSBresult()); // separate variable used to cast to a signed int
                                                                                                   // for the float cast to work correctly

    return (((float) data) / 32768) * range;
}



//****************************************************************************
//! Get Temperature Measurement in degrees C
//!
//! Currently the 'Typical' Electrical Characteristics (ECHAR) of the device are set
//! in the header file. If the device has been calibrated and different ECHAR values
//! are found, please edit the ECHAR values in the header file for more accurate
//! temperature measurement. The header file also contains more information on ECHAR values.
//****************************************************************************
float TMAG5x73getMeasurementTEMP()
{
    uint16_t tADC_T = ((uint16_t) TMAG5x73getTempMSBresult() << 8) | ((uint16_t) TMAG5x73getTempLSBresult());

    float temp_val = ECHAR_T_SENS_T0 + (((((float) tADC_T) - ECHAR_T_ADC_T0)) / ECHAR_T_ADC_RES);
    return temp_val;
}



//****************************************************************************
//! Get Internal Angle Measurement in Degrees
//!
//! Returns a float containing the degree value converted from the ANGLE_RESULT register.
//! The value corresponds to the calculated angle created by the two magnetic flux axis
//! measurements selected by the ANGLE_EN bits.
//!
//! For the angle to be properly measured, the two axes selected by ANGLE_EN must share the
//! same selected range.
//****************************************************************************
float TMAG5x73getMeasurementANGLE()
{
    uint16_t data = ((uint16_t) TMAG5x73getAngleMSBresult() << 8) | ((uint16_t) TMAG5x73getAngleLSBresult());
    float angle = ((float) data / 16);
    return angle;
}



//****************************************************************************
//! Get Internal Magnitude Measurement in mT
//!
//! Returns a float containing the mT value converted from the MAGNITUDE_RESULT register.
//! The value corresponds to the calculated magnitude created by the two magnetic flux axis
//! measurements selected by the ANGLE_EN bits.
//!
//! For the magnitude to be properly measured, the two axes selected by ANGLE_EN must share the
//! same selected range.
//****************************************************************************
float TMAG5x73getMeasurementMAG()
{
    uint16_t data = TMAG5x73getMAGresult();
    float magnitude = (((float) data)/8192) * TMAG5x73getXYrange() * 4;
    return magnitude;

}



//****************************************************************************
//! Get Magnetic Measurements in mT
//!
//! Takes in a size 3 array of floats and updates its measurements of the
//! three magnetic axes in mT. (order XYZ)
//!
//! INPUT ARRAY MUST BE SIZE 3 (or at least have meas_arr to meas_arr + 2 within scope)
//****************************************************************************
void TMAG5x73getMagMeasurements(float meas_arr[])
{

    uint8_t i;

    // Array to store ranges for coordinates in the order XYZ
    uint16_t ranges[3] = {40,40,40}; // The default value for coordinate ranges is 40 mT (A1)
    ranges[0] = TMAG5x73getXYrange();
    ranges[1] = TMAG5x73getXYrange();
    ranges[2] = TMAG5x73getZrange();

    int16_t data;

    for (i=0; i<6; i+=2)
    {
        data = (TMAG5x73readSingleRegister(X_MSB_RESULT_ADDRESS + i, Trigger_Conversion_Enabled)) << 8 | TMAG5x73readSingleRegister(X_MSB_RESULT_ADDRESS + i + 1, Trigger_Conversion_Enabled); // read in
        meas_arr[i/2] = (((float) data) / 32768) * ranges[i/2];
    }
}



//****************************************************************************
//****************************************************************************
//
// Get Results/Measurement Functions (1-byte I2C Reads Only)
//
//****************************************************************************
//****************************************************************************


//****************************************************************************
//! dataChannels - Data channels (T, X, Y, Z) that are enabled. If using more than
//!                one channel, add up the values (i.e., if using X, Y, and Z,
//!                dataChannels = 0x0E)
//!                0x01 - T channel is enabled
//!                0x02 - X channel is enabled
//!                0x04 - Y channel is enabled
//!                0x08 - Z channel is enabled
//!
//! bitRead     - Data is 8-bit/16-bit
//!               0x00 - 8-bit data read
//!               0x01 - 16-bit data read
//!
//! crc_en      - CRC calculation is enabled
//!               0x00 - CRC calculation is disabled
//!               0x01 - CRC calculation is enabled
//!
//! xy_range    - the X_Y_RANGE in mT values can either be 40/80 for A1 or 133/266 for A2
//!
//! z_range     - the Z_RANGE in mT values can either be 40/80 for A1 or 133/266 for A2
//!
//! data[]      - an empty array that will hold all data read by this function,
//!               data will placed in this array in the order T, X, Y, Z when
//!               multiple channels are enabled
//!
//! NOTE: At least one data channel must be enabled
//! NOTE: When using the 16-bit read for the TMAG5273 only up to 3 data channels
//!       can be read if CRC is enabled
//****************************************************************************
void TMAG5x73getOneByteMeasurements(const uint8_t dataChannels, const uint8_t bitRead, const uint16_t xy_range, const uint16_t z_range, float data[])
{
    uint8_t tempDataArray[10] = {};
    uint8_t numChannels = 0;
    int16_t x_mT, y_mT, z_mT, T_degC;

    if(dataChannels & 0x01) numChannels++;
    if(dataChannels & 0x02) numChannels++;
    if(dataChannels & 0x04) numChannels++;
    if(dataChannels & 0x08) numChannels++;

    TMAG5x73oneByteRead(numChannels, bitRead, 0x00, tempDataArray);

    if (bitRead == 0x00) // 8-bit data read
    {
        if((dataChannels & 0x01) == 0x01) // T is enabled
        {
            T_degC = (int16_t) tempDataArray[0];
            data[0] = ECHAR_T_SENS_T0 + (( 256 * (((float) T_degC) - ECHAR_T_ADC_T0)) / ECHAR_T_ADC_RES);

            if((dataChannels & 0x02) == 0x02) // X is enabled
            {
                x_mT = (int16_t) tempDataArray[1] << 8;
                data[1] = (((float) x_mT) / 32768) * xy_range;

                if((dataChannels & 0x04) == 0x04) // Y is enabled
                {
                    y_mT = (int16_t) tempDataArray[2] << 8;
                    data[2] = (((float) y_mT) / 32768) * xy_range;

                    if((dataChannels & 0x08) == 0x08) // Z is enabled
                    {
                        z_mT = (int16_t) tempDataArray[3] << 8;
                        data[3] = (((float) z_mT) / 32768) * z_range;
                    }
                }
                else // Y is not enabled
                {
                    if((dataChannels & 0x08) == 0x08) // Z is enabled
                    {
                        z_mT = (int16_t) tempDataArray[2] << 8;
                        data[2] = (((float) z_mT) / 32768) * z_range;
                    }
                }
            }
            else // X is not enabled
            {
                if((dataChannels & 0x04) == 0x04) // Y is enabled
                {
                    y_mT = (int16_t) tempDataArray[1] << 8;
                    data[1] = (((float) y_mT) / 32768) * xy_range;

                    if((dataChannels & 0x08) == 0x08) // Z is enabled
                    {
                        z_mT = (int16_t) tempDataArray[2] << 8;
                        data[2] = (((float) z_mT) / 32768) * z_range;
                    }
                }
                else // Y is not enabled
                {
                    if((dataChannels & 0x08) == 0x08) // Z is enabled
                    {
                        z_mT = (int16_t) tempDataArray[1] << 8;
                        data[1] = (((float) z_mT) / 32768) * z_range;
                    }
                }
            }
        }
        else // T is not enabled
        {
            if((dataChannels & 0x02) == 0x02) // X is enabled
            {
                x_mT = (int16_t) tempDataArray[0] << 8;
                data[0] = (((float) x_mT) / 32768) * xy_range;

                if((dataChannels & 0x04) == 0x04) // Y is enabled
                {
                    y_mT = (int16_t) tempDataArray[1] << 8;
                    data[1] = (((float) y_mT) / 32768) * xy_range;

                    if((dataChannels & 0x08) == 0x08) // Z is enabled
                    {
                        z_mT = (int16_t) tempDataArray[2] << 8;
                        data[2] = (((float) z_mT) / 32768) * z_range;
                    }
                }
                else // Y is not enabled
                {
                    if((dataChannels & 0x08) == 0x08) // Z is enabled
                    {
                        z_mT = (int16_t) tempDataArray[1] << 8;
                        data[1] = (((float) z_mT) / 32768) * z_range;
                    }
                }
            }
            else // X is not enabled
            {
                if((dataChannels & 0x04) == 0x04) // Y is enabled
                {
                    y_mT = (int16_t) tempDataArray[0] << 8;
                    data[0] = (((float) y_mT) / 32768) * xy_range;

                    if((dataChannels & 0x08) == 0x08) // Z is enabled
                    {
                        z_mT = (int16_t) tempDataArray[1] << 8;
                        data[1] = (((float) z_mT) / 32768) * z_range;
                    }
                }
                else // Y is not enabled
                {
                    if((dataChannels & 0x08) == 0x08) // Z is enabled
                    {
                        z_mT = (int16_t) tempDataArray[0] << 8;
                        data[0] = (((float) z_mT) / 32768) * z_range;
                    }
                }
            }
        }
    }
    else // 16-bit data read
    {
        if((dataChannels & 0x01) == 0x01) // T is enabled
        {
            T_degC = ((int16_t) tempDataArray[0] << 8) | (int16_t) tempDataArray[1];
            data[0] = ECHAR_T_SENS_T0 + (((((float) T_degC) - ECHAR_T_ADC_T0)) / ECHAR_T_ADC_RES);

            if((dataChannels & 0x02) == 0x02) // X is enabled
            {
                x_mT = ((int16_t) tempDataArray[2] << 8) | (int16_t) tempDataArray[3];
                data[1] = (((float) x_mT) / 32768) * xy_range;

                if((dataChannels & 0x04) == 0x04) // Y is enabled
                {
                    y_mT = ((int16_t) tempDataArray[4] << 8) | (int16_t) tempDataArray[5];
                    data[2] = (((float) y_mT) / 32768) * xy_range;

                    if((dataChannels & 0x08) == 0x08) // Z is enabled
                    {
                        z_mT = ((int16_t) tempDataArray[6] << 8) | (int16_t) tempDataArray[7];
                        data[3] = (((float) z_mT) / 32768) * z_range;
                    }
                }
                else // Y is not enabled
                {
                    if((dataChannels & 0x08) == 0x08) // Z is enabled
                    {
                        z_mT = ((int16_t) tempDataArray[4] << 8) | (int16_t) tempDataArray[5];
                        data[2] = (((float) z_mT) / 32768) * z_range;
                    }
                }
            }
            else // X is not enabled
            {
                if((dataChannels & 0x04) == 0x04) // Y is enabled
                {
                    y_mT = ((int16_t) tempDataArray[2] << 8) | (int16_t) tempDataArray[3];
                    data[1] = (((float) y_mT) / 32768) * xy_range;

                    if((dataChannels & 0x08) == 0x08) // Z is enabled
                    {
                        z_mT = ((int16_t) tempDataArray[4] << 8) | (int16_t) tempDataArray[5];
                        data[2] = (((float) z_mT) / 32768) * z_range;
                    }
                }
                else // Y is not enabled
                {
                    if((dataChannels & 0x08) == 0x08) // Z is enabled
                    {
                        z_mT = ((int16_t) tempDataArray[2] << 8) | (int16_t) tempDataArray[3];
                        data[1] = (((float) z_mT) / 32768) * z_range;
                    }
                }
            }
        }
        else // T is not enabled
        {
            if((dataChannels & 0x02) == 0x02) // X is enabled
            {
                x_mT = ((int16_t) tempDataArray[0] << 8) | (int16_t) tempDataArray[1];
                data[0] = (((float) x_mT) / 32768) * xy_range;

                if((dataChannels & 0x04) == 0x04) // Y is enabled
                {
                    y_mT = ((int16_t) tempDataArray[2] << 8) | (int16_t) tempDataArray[3];
                    data[1] = (((float) y_mT) / 32768) * xy_range;

                    if((dataChannels & 0x08) == 0x08) // Z is enabled
                    {
                        z_mT = ((int16_t) tempDataArray[4] << 8) | (int16_t) tempDataArray[5];
                        data[2] = (((float) z_mT) / 32768) * z_range;
                    }
                }
                else // Y is not enabled
                {
                    if((dataChannels & 0x08) == 0x08) // Z is enabled
                    {
                        z_mT = ((int16_t) tempDataArray[2] << 8) | (int16_t) tempDataArray[3];
                        data[1] = (((float) z_mT) / 32768) * z_range;
                    }
                }
            }
            else // X is not enabled
            {
                if((dataChannels & 0x04) == 0x04) // Y is enabled
                {
                    y_mT = ((int16_t) tempDataArray[0] << 8) | (int16_t) tempDataArray[1];
                    data[0] = (((float) y_mT) / 32768) * xy_range;

                    if((dataChannels & 0x08) == 0x08) // Z is enabled
                    {
                        z_mT = ((int16_t) tempDataArray[2] << 8) | (int16_t) tempDataArray[3];
                        data[1] = (((float) z_mT) / 32768) * z_range;
                    }
                }
                else // Y is not enabled
                {
                    if((dataChannels & 0x08) == 0x08) // Z is enabled
                    {
                        z_mT = ((int16_t) tempDataArray[0] << 8) | (int16_t) tempDataArray[1];
                        data[0] = (((float) z_mT) / 32768) * z_range;
                    }
                }
            }
        }
    }
}



//****************************************************************************
//****************************************************************************
//
// Get Range Functions
//
//****************************************************************************
//****************************************************************************


//****************************************************************************
//! Get and return the integer value of the X_Y_RANGE bits for an axis
//!
//! Returns an unsigned 16-bit integer value of the X axis range in mT.
//****************************************************************************
uint16_t TMAG5x73getXYrange()
{
    // Get SENSOR_CONFIG_2 and isolate X_Y_RANGE bits.
    uint8_t config = TMAG5x73readSingleRegister(SENSOR_CONFIG_2_ADDRESS, Trigger_Conversion_Enabled) & SENSOR_CONFIG_2_X_Y_RANGE_MASK >> 1;
    uint16_t range;
    if (TMAG5x73getVersion() == 2)
    {
        // range values for TMAG5x73A2
        range = 133;
        if (config == 0x1) range = 266; // If examined bits equal 1b, range is set to 266 mT (for A2)
        else range = 133; // If examined bits equal 0b, range is set to 133 mT (for A2)
    }
    else
    {
        // range values for TMAG5x73A1
        range = 40;
        if (config == 0x1) range = 80; // If examined bits equal 1b, range is set to 80 mT (for A1)
        else range = 40; // If examined bits equal 0b, range is set to 40 mT (for A1)
    }
    return range;
}



//****************************************************************************
//! Get and return the integer value of the Z_RANGE bits for an axis
//!
//! Returns an unsigned 16-bit integer value of the Z axis range in mT.
//****************************************************************************
uint16_t TMAG5x73getZrange()
{
    // Get SENSOR_CONFIG and isolate Z_RANGE bits, shifting them to LSB.
    uint8_t config = TMAG5x73readSingleRegister(SENSOR_CONFIG_2_ADDRESS, Trigger_Conversion_Enabled) & SENSOR_CONFIG_2_Z_RANGE_MASK;
    uint16_t range;
    if (TMAG5x73getVersion() == 2)
    {
        // range values for TMAG5173A2
        range = 133;
        if (config == 0x1) range = 266; // If examined bits equal 1b, range is set to 266 mT (for A2)
        else range = 133; // If examined bits equal 0b, range is set to 133 mT (for A2)
    }
    else
    {
        // range values for TMAG5173A1
        range = 40;
        if (config == 0x1) range = 80; // If examined bits equal 1b, range is set to 80 mT (for A1)
        else range = 40; // If examined bits equal 0b, range is set to 40 mT (for A1)
    }
    return range;
}



//****************************************************************************
//****************************************************************************
//
// Get Device Info Functions
//
//****************************************************************************
//****************************************************************************

//****************************************************************************
//! Get TMAG5x73 Version (A1 or A2)
//!
//! Sends a read command for TEST_CONFIG and returns the VER field bit.
//!      VER == 0x00 --> TMAG5x73APL
//!      VER == 0x01 --> TMAG5x73A1
//!      VER == 0x02 --> TMAG5x73A2
//****************************************************************************
uint8_t TMAG5x73getVersion()
{ return TMAG5x73readSingleRegister(DEVICE_ID_ADDRESS, Trigger_Conversion_Enabled) & DEVICE_ID_VER_MASK; }



//****************************************************************************
//! Check if CRC is enabled
//!
//! Sends a read command for TEST_CONFIG and returns the whether the CRC_DIS field
//! corresponds to an enabled CRC or not. (1b == enabled)
//****************************************************************************
uint8_t TMAG5x73isCRCenabled()
{
    return ((TMAG5x73readSingleRegister(DEVICE_CONFIG_1_ADDRESS, Trigger_Conversion_Enabled) & DEVICE_CONFIG_1_CRC_EN_MASK) >> 7) == 1;
}



//****************************************************************************
//****************************************************************************
//
// Offset and Gain Correction Functions
//
//****************************************************************************
//****************************************************************************

//****************************************************************************
//! Set Magnetic Sensor Offset Correction (8-Bit inputs)
//!
//! Configures the MAG_OFFSET_CONFIG_1 (0x0A) and MAG_OFFSET_CONFIG_2 (0x0B)
//! according to input values (check datasheet pgs. 31 and 32 (TMAG5273) / pgs.
//! 39 and 40 (TMAG5173))
//!
//! Keep in mind the axes that offset1 and offset2 are applied to depend on ANGLE_EN (0x01F-E)
//!              ANGLE_EN value | offset1 axis | offset2 axis
//!                   0x00            none           none
//!                   0x01              X              Y
//!                   0x02              Y              Z
//!                   0x03              X              Z
//!
//! offset1_bits - 8-bit setting for OFFSET_VALUE1 [must be no greater than 0x7F]
//! offset2_bits - 8-bit setting for OFFSET_VALUE2 [must be no greater than 0x7F]
//!
//! Conversion between MAG_OFFSET_CONFIG register and actual offset deltas is on datasheet
//! pgs. 24 and 25 (TMAG5273) / pgs. 25 and 26 (TMAG5173)
//****************************************************************************
void TMAG5x73setMagOffsetIn8Bit(uint8_t offset1_bits, uint8_t offset2_bits)
{
    if (offset1_bits > 0xFF || offset2_bits > 0xFF) return;

    uint16_t input;
    // MAG_OFFSET_CONFIG (0x12) has all 16 bits assigned according to the two input variables
    input = offset1_bits;
    TMAG5x73writeToSingleRegister(MAG_OFFSET_CONFIG_1_ADDRESS, input);

    input = offset2_bits;
    TMAG5x73writeToSingleRegister(MAG_OFFSET_CONFIG_2_ADDRESS, input);
}



//****************************************************************************
//! Set Magnetic Sensor Offset Correction (mT inputs)
//!
//! Configures the MAG_OFFSET_CONFIG_1 (0x0A) and MAG_OFFSET_CONFIG_2 (0x0B)
//! according to input values (check datasheet pgs. 31 and 32 (TMAG5273) / pgs.
//! 39 and 40 (TMAG5173))
//!
//! Keep in mind the axes that offset1 and offset2 are applied to depend on ANGLE_EN (0x01F-E)
//!              ANGLE_EN value | offset1 axis | offset2 axis
//!                   0x00            none           none
//!                   0x01              X              Y
//!                   0x02              Y              Z
//!                   0x03              X              Z
//!
//! offset_select - setting for OFFSET_SELECTION [must be no greater than 0x03]
//! offset1_delta - mT value of offset shift for the selected offset1 axis
//! offset2_delta - mT value of offset shift for the selected offset2 axis
//! (an axis with a +2mT error should have a -2mT value entered into its offset variable)
//!
//! Valid offset values for each possible range setting:
//!          SET AXIS RANGE   |  VALID OFFSET VALUES RANGE  (mT)
//!            40 mT (A1)               -2.5  to 2.48
//!            80 mT (A1)               -5    to 4.96
//!           133 mT (A2)               -8.31 to 8.24
//!           266 mT (A2)              -16.62 to 16.49
//!
//! (valid values were rounded to the hundredths place closer to zero to prevent including invalid
//! values in the range, the exact corresponding offset based on the MAG_OFFSET_CONFIG register can be
//! calculated using the eqns. provided on datasheet pgs. 25 and 26)
//****************************************************************************
void TMAG5x73setMagOffsetInmT(const uint8_t offset_select, const float offset1_delta , const float offset2_delta)
{
    // Checks if offset_select value is valid and offset correction is enabled
    if((offset_select == 0x00) || (offset_select > 0x03)) return;


    uint16_t delta1_range = 0;
    uint16_t delta2_range = 0;

    switch (offset_select)
    {
        case 0x01 :
            delta1_range = TMAG5x73getXYrange();
            delta2_range = TMAG5x73getXYrange();
            break;
        case 0x02 :
            delta1_range = TMAG5x73getXYrange();
            delta2_range = TMAG5x73getZrange();
            break;
        case 0x03 :
            delta1_range = TMAG5x73getXYrange();
            delta2_range = TMAG5x73getZrange();
            break;
    }

    // take the rounded inverse of the eqn to get the signed 8-bit int value for
    // the desired offset and then convert that to 7-bits to send to the device
    // (the 7-bit value is put in the 7 LSBs of an 8-bit unsigned int)
    int8_t offset1_value = round((offset1_delta * 2048) / delta1_range);
    int8_t offset2_value = round((offset2_delta * 2048) / delta2_range);


    uint8_t offset1_bits = ((uint8_t) offset1_value) & 0x7F ; // assign the 7 LSBs
    offset1_bits = offset1_bits | ((((uint16_t) offset1_value) & 0x100) >> 1); // shift signing bit to 8th LSB
    uint8_t offset2_bits = ((uint8_t) offset2_value) & 0x7F ;
    offset2_bits = offset2_bits | ((((uint16_t) offset2_value) & 0x100) >> 1);

    TMAG5x73setMagOffsetIn8Bit(offset1_bits, offset2_bits);
}



//****************************************************************************
//! Set Magnetic Gain Adjustment using 8-bit input
//! Configures the MAG_GAIN_CONFIG (0x09) according to 8-bit input value
//! (check datasheet pg. 30 (TMAG5273) / pg. 38 (TMAG5173) for conversion)
//!
//! channel - selection of a particular axis for amplitude correction (must be no greater than 0x01)
//!           0x00 - 1st Channel | 0x01 - 2nd Channel
//! gain_bits - 11-bit gain value to adjust selected axis value (must be no greater than 0xFF)
//!             gain calculated as (entered_value/1024)
//****************************************************************************
void TMAG5x73setMagGainConfigIn8Bit(uint8_t channel, uint16_t gain_bits)
{
    // Checks if inputs value are valid
    if (channel > 0x01 || gain_bits > 0xFF) return;

    uint16_t input;
    // MAG_OFFSET_CONFIG (0x11) has all 16 bits assigned according to the two input variables
    input =  (TMAG5x73readSingleRegister(SENSOR_CONFIG_2_ADDRESS, Trigger_Conversion_Enabled) & ~(SENSOR_CONFIG_2_MAG_GAIN_CH_MASK));
    input = input | (channel << 4);
    TMAG5x73writeToSingleRegister(SENSOR_CONFIG_2_ADDRESS, input);

    input =  gain_bits;
    TMAG5x73writeToSingleRegister(MAG_GAIN_CONFIG_ADDRESS, input);
}



//****************************************************************************
//! Set Magnetic Gain Adjustment using decimal input
//!
//! Configures the MAG_GAIN_CONFIG (0x11) according to decimal input values from
//! 0 to 1 (exclusive). See Section 8.2.3.2 Detailed Design Procedure in the
//! datasheet for more information on how to calculated the Magnetic Gain Adjustment
//!
//! channel - selection of a particular axis for amplitude correction (must be no greater than 0x01)
//!           0x00 - 1st Channel | 0x01 - 2nd Channel
//! gain_value - float input of the desired gain value from 0 to 1 (exclusive) based
//!              off the formula, 'user entered value in decimal/256' to be assigned
//!              with the selected axis
//!
//! NOTE: The variable gain_value should have at least 3 significant digits, unless
//!       the desired value has an exact value that is less than 3 significant digits.
//****************************************************************************
void TMAG5x73setMagGainConfigInDecimal(uint8_t channel, float gain_value)
{
    if (gain_value < 0 || gain_value >= 1) return;
    uint16_t gain_bits = round(gain_value * 256); 
    TMAG5x73setMagGainConfigIn8Bit(channel, gain_bits);
}



//****************************************************************************
//****************************************************************************
//
// Supplemental Functions
//
//****************************************************************************
//****************************************************************************


//****************************************************************************
//! Calculate Angle and Magnitude using CORDIC for two axes
//! Takes in a float array of at least size 2, two magnetic axis measurements, and their
//! shared range and changes indexes 0 and 1 of the array to the calculated angle and
//! magnitude using CORDIC.
//!
//! CORDIC_results[] - float array of at least size 2, will have indexes 0 and 1 replaced
//!                    with the calculated angle and magnitude, respectively
//! numerator - magnetic measurement result pulled from the register of the vertical axis
//! denominator - magnetic measurement result pulled from the register of the horizontal axis
//! range - the range in mT shared by both axes (axes cannot have different set ranges or CORDIC will not
//!         be accurate)
//!
//! For angle measurements to match the in-built CORDIC on the device, match the numerator and denominator
//! for the associated ANGLE_EN value below, while also ensuring both axes share the same range (in mT):
//!
//!              ANGLE_EN value | numerator axis | denominator axis
//!                   0x00             none             none
//!                   0x01              Y                X
//!                   0x02              Z                Y
//!                   0x03              Z                X
//!
//! The returned angle will be interpreted as 0deg on the positive denominator axis and 90deg on the positive numerator axis
//!
//! For more information on CORDIC algorithms please watch the "CORDIC algorithm for angle calculations" video
//! provided by Texas Instruments: https://training.ti.com/cordic-algorithm-angle-calculations
//****************************************************************************
void calcCORDIC(float CORDIC_results[], int16_t numerator, int16_t denominator, uint16_t range,
                        int16_t iteration_length)
{
    if (iteration_length > 16 || iteration_length < 1) return;

    int32_t angle_readings[3];
    int32_t ANGLE_CALC_32;
    int32_t MAG_CALC_CORDIC;

    atan2CORDIC(numerator, denominator, iteration_length, angle_readings);

    ANGLE_CALC_32 = angle_readings[0];
    MAG_CALC_CORDIC = angle_readings[1];

    CORDIC_results[0] = (((float) ANGLE_CALC_32)/65536) * 360 / 65536; // angle result
    CORDIC_results[1] = ((float) MAG_CALC_CORDIC) * range / 32768; // mag result
}


//****************************************************************************
//! atan2 + magnitude calculation using CORDIC algorithm
//!
//! Implementation of the CORDIC algorithm without result value conversion for
//! faster use with functions repeatedly using the CORDIC algorithm (see the
//! planeAngles function).
//!
//! numerator - magnetic measurement result pulled from the register of the vertical axis
//! denominator - magnetic measurement result pulled from the register of the horizontal axis
//! iteration_length - the number of "rotations" to be made in the calculation, more generally
//!                    means a more accurate calculation (max amount is 16)
//! results - int32_t array of at lease size 2. results[0] will store the unconverted angle
//!           value calculated by the algorithm. results[1] will store the unconverted
//!           magnitude value. See the calcCORDIC function for how to convert these values
//!           using the lookup tables included.
//!
//! For more information on CORDIC algorithms please watch the "CORDIC algorithm for angle calculations" video
//! provided by Texas Instruments: https://training.ti.com/cordic-algorithm-angle-calculations
//****************************************************************************
void atan2CORDIC(int16_t numerator, int16_t denominator, int16_t iteration_length, int32_t results[])
{
    if (iteration_length > 16 || iteration_length < 1) return;

    int i=0;
    int32_t num_old, den_old, num, den;
    uint32_t angle;
    num = num_old = numerator;
    den = den_old = denominator;

    if (den < 0) angle = 0x80000000;
    else angle = 0;

    for(i = 0 ; i < iteration_length ; i++)
    {

        if (((den >= 0) && (num < 0)) ||((den < 0) && (num >= 0)))
        {
            den = den - (num_old >> i);
            num = num + (den_old >> i);
            angle = angle - atanArray32[i];
        }
        else
        {
            den = den + (num_old >> i);
            num = num - (den_old >> i);
            angle = angle + atanArray32[i];
        }
        den_old = den;
        num_old = num;
    }
    results[0] = angle;
    if (den < 0) den = -den;
    results[1]= (((int64_t)den)*magArray[i-1])>>(15+16);
}



//****************************************************************************
//! Plane Angle Calculations
//!
//! Takes in the result registers of three axes (assuming all share the same range)
//! and an output array of at lease size 3.
//!
//! results[0] will correspond to the YZ plane angle value.
//! results[1] will correspond to the XZ plane angle value.
//! results[2] will correspond to the XY plane angle value (if enabled).
//****************************************************************************
void planeAngles(int16_t axisX, int16_t axisY, int16_t axisZ,  int32_t results[])
{
    axisX >>= 1;
    axisY >>= 1;
    axisZ >>= 1;
    int32_t angle_mag_readings[2];
    uint32_t xz_magnitude, yz_magnitude;


    atan2CORDIC(axisZ, axisY, 10, angle_mag_readings);
    yz_magnitude=angle_mag_readings[1];
    atan2CORDIC(yz_magnitude, axisX, 10, angle_mag_readings);
    results[0]=angle_mag_readings[0];

    atan2CORDIC(axisZ, axisX, 10, angle_mag_readings);
    xz_magnitude=angle_mag_readings[1];
    atan2CORDIC(xz_magnitude, axisY, 10, angle_mag_readings);
    results[1]=angle_mag_readings[0];

//#define CALCULATE_XY_PLANE_ANGLE
#ifdef CALCULATE_XY_PLANE_ANGLE
    uint32_t xy_magnitude;
    atan2CORDIC(axisX, axisY, 10, angle_mag_readings);
    xy_magnitude=angle_mag_readings[1];
    atan2CORDIC(xy_magnitude, axisZ, 10, angle_mag_readings);
    results[2]=angle_mag_readings[0];
#endif
}


//****************************************************************************
//! Conversion to Spherical Coordinates
//!
//! Takes in the result registers of three axes (assuming all share the same range)
//! and an output array of at lease size 3. The three axis measurements are converted
//! into spherical coordinates with phi on the axis1 + axis2 plane.
//!
//! results[0] will correspond to the radius value.
//! results[1] will correspond to the phi angle value on the axis1 + axis2 plane.
//! results[2] will correspond to the theta angle value.
//****************************************************************************
void convertToSpherical(int16_t axis1, int16_t axis2, int16_t axis3,  int32_t results[])
{
    int32_t angle_mag_readings[2];
    uint32_t xy_magnitude;


    results[0]= mag3D(axis1, axis2, axis3);

    atan2CORDIC(axis2, axis1, 10, angle_mag_readings);
    results[1]=angle_mag_readings[0];
    xy_magnitude=angle_mag_readings[1];

    atan2CORDIC(xy_magnitude, axis3, 10, angle_mag_readings);
    results[2]=angle_mag_readings[0];

}


//****************************************************************************
//! Conversion to Cylindrical Coordinates
//!
//! Takes in the result registers of three axes (assuming all share the same range)
//! and an output array of at lease size 3. The three axis measurements are converted
//! into cylindrical coordinates with the cylinder oriented along axis3.
//!
//! results[0] will correspond to the radius value.
//! results[1] will correspond to the theta angle value of the axis1 + axis2 plane.
//! results[2] will correspond to the z value.
//****************************************************************************
void convertToCylindrical(int16_t axis1, int16_t axis2, int16_t axis3,  int32_t results[])
{
    int32_t angle_mag_readings[2];

    atan2CORDIC(axis2, axis1, 10, angle_mag_readings);
    results[0]=angle_mag_readings[1];
    results[1]=angle_mag_readings[0];
    results[2]=axis3;
}


//****************************************************************************
//! 3-axis Magnitude Calculation
//!
//! Takes in three result registers and returns the magnitude of a vector created
//! by those three input values.
//****************************************************************************
uint32_t mag3D(int16_t axis1, int16_t axis2, int16_t axis3)
{
    uint32_t temp1, temp2,temp3;

    temp3= axis3>>1;
    temp3=temp3*temp3;

    temp2=axis2>>1;
    temp2= temp2*temp2;

    temp1= axis1>>1;
    temp1=temp1*temp1;


    temp1=temp3+temp2+temp1;
    temp1=isqrt32(temp1)>>16;
    return temp1<<1;
}




//****************************************************************************
//! Piecewise Linearization Function - Axis Measurements (Registers)
//! Takes in known measurements with their associated errors and uses a linearization of them
//! to calculate an estimation of an unknown measured value with error absent.
//!
//! knownValue[] - sorted array of measured register values with their error known (MUST BE SORTED LOW to HIGH)
//! knownError[] - array of known error values (in mT) for the corresponding knownValue[] indices
//! known_length - length of the knownValue[] and knownError[] arrays
//! measValue - the measured register value with unknown error the linearization will be applied to.
//! range - range of the axis the registers to be linearized are taken from
//****************************************************************************
float piecewiseLinearizationRegister(int16_t knownValue[], float knownError[], uint16_t known_length, int16_t measValue, uint16_t range)
{

    uint16_t top_index;

    for (top_index = 1; top_index < known_length - 1 ; top_index++)
    {
        if (measValue < knownValue[top_index]) break;
    }

    float coord_diff, error_diff, slope, estimated_error/*, linztn_value*/;
    float bottomValue_f = TMAG5x73resultRegisterTomT(knownValue[top_index - 1], range);
    float measValue_f = TMAG5x73resultRegisterTomT(measValue, range);

    coord_diff = ((float) (knownValue[top_index] - knownValue[top_index - 1]) / 32768) * range;
    error_diff = knownError[top_index] - knownError[top_index - 1];
    slope = error_diff / coord_diff;

    estimated_error = (measValue_f - bottomValue_f)*slope + knownError[top_index - 1];
    return measValue_f - estimated_error;
}



//****************************************************************************
//! Piecewise Linearization Function - Axis Measurements (mT values)
//! Takes in known measurements with their associated errors and uses a linearization of them
//! to calculate an estimation of an unknown measured value with error absent.
//!
//! knownValue[] - sorted array of measured mT values with their error known (MUST BE SORTED LOW to HIGH)
//! knownError[] - array of known error values (in mT) for the corresponding knownValue[] indices
//! known_length - length of the knownValue[] and knownError[] arrays
//! measValue - the measured mT value with unknown error the linearization will be applied to.
//! range - range of the axis the registers to be linearized are taken from
//****************************************************************************
float piecewiseLinearizationFloat(float knownValue[], float knownError[], uint16_t known_length, float measValue)
{
    uint16_t top_index;
    float coord_diff, error_diff;
    float slope, estimated_error;

    for (top_index = 1; top_index < known_length - 1 ; top_index++)
    {
        if (measValue < knownValue[top_index]) break;
    }

    coord_diff = knownValue[top_index] - knownValue[top_index - 1];
    error_diff = knownError[top_index] - knownError[top_index - 1];
    slope =  error_diff / coord_diff;
    estimated_error = (measValue - knownValue[top_index - 1])*slope + knownError[top_index - 1];

    return measValue - estimated_error;
}



//****************************************************************************
//! Piecewise Linearization Function - Angle Measurements (Register values)
//! Takes in known angle measurements with their associated errors and uses a linearization of them
//! to calculate an estimation of an unknown measured angle with error absent.
//!
//! knownValue[] - sorted array of measured angle register values with their error known (MUST BE SORTED LOW to HIGH)
//! knownError[] - array of known error values (in degrees) for the corresponding knownValue[] indices
//! known_length - length of the knownValue[] and knownError[] arrays
//! measValue - the measured angle register value with unknown error the linearization will be applied to.
//****************************************************************************
float piecewiseLinearizationAngle(uint16_t knownAngle[], float knownError[], uint16_t known_length, uint16_t measAngle)
{

    uint16_t top_index = known_length;
    uint16_t bottom_index;
    float slope, coord_diff, error_diff, estimated_error, linztn_angle;

    if (measAngle >= knownAngle[0])
    {
        for (top_index = 1; top_index < known_length ; top_index++)
        {
            if (measAngle < knownAngle[top_index]) break;
        }
    }
    bottom_index = top_index - 1;
    if (top_index == known_length)
    {
        top_index = 0;
        coord_diff = TMAG5x73angleRegisterToDeg((knownAngle[top_index] + 0x1680) - knownAngle[bottom_index]);
    }
    else
    {
        coord_diff = TMAG5x73angleRegisterToDeg(knownAngle[top_index] - knownAngle[bottom_index]);
    }

    float bottomAngle_f = TMAG5x73angleRegisterToDeg(knownAngle[bottom_index]);
    float measAngle_f = TMAG5x73angleRegisterToDeg(measAngle);

    error_diff = knownError[top_index] - knownError[bottom_index];
    slope = error_diff / coord_diff;
    estimated_error = (measAngle_f - bottomAngle_f)*slope + knownError[bottom_index];
    linztn_angle = measAngle_f - estimated_error;
    if (linztn_angle >= 360) linztn_angle -= 360;

    return linztn_angle;
}



//****************************************************************************
//! Piecewise Linearization Function - Angle Measurements (mT values)
//! Takes in known angle measurements with their associated errors and uses a linearization of them
//! to calculate an estimation of an unknown measured angle value with error absent.
//!
//! knownValue[] - sorted array of measured degree values with their error known (MUST BE SORTED LOW to HIGH)
//! knownError[] - array of known error values (in degrees) for the corresponding knownValue[] indices
//! known_length - length of the knownValue[] and knownError[] arrays
//! measValue - the measured degree value with unknown error the linearization will be applied to.
//****************************************************************************
float piecewiseLinearizationAngleFloat(float knownAngle[], float knownError[], uint16_t known_length, float measAngle)
{

    uint16_t top_index = known_length;
    uint16_t bottom_index;
    float slope, coord_diff, error_diff, estimated_error, linztn_angle;

    if (measAngle >= knownAngle[0])
    {
        for (top_index = 1; top_index < known_length ; top_index++)
        {
            if (measAngle < knownAngle[top_index]) break;
        }
    }
    bottom_index = top_index - 1;
    if (top_index == known_length)
    {
        top_index = 0;
        coord_diff = (knownAngle[top_index] + 360) - knownAngle[bottom_index];
    }
    else
    {
        coord_diff = knownAngle[top_index] - knownAngle[bottom_index];
    }

    error_diff = knownError[top_index] - knownError[bottom_index];
    slope = error_diff / coord_diff;
    estimated_error = (measAngle - knownAngle[bottom_index])*slope + knownError[bottom_index];
    linztn_angle = measAngle - estimated_error;
    if (linztn_angle >= 360) linztn_angle -= 360;

    return linztn_angle;
}





//****************************************************************************
//****************************************************************************
//
// Helper Functions
//
//****************************************************************************
//****************************************************************************


//****************************************************************************
//! 32-bit Square Root Function
//!
//! Takes the square of the input integer h and returns a 32-bit value that can
//! be converted to the floating point value by dividing it by 65536.
//****************************************************************************
uint32_t isqrt32(uint32_t h)
{
    uint32_t x;
    uint32_t y;
    int i;
    x = y = 0;
    for (i = 0;  i < 32;  i++)
    {
        x = (x << 1) | 1;
        if (y < x) x -= 2;
        else y -= x;
        x++;
        y <<= 1;
        if ((h & 0x80000000)) y |= 1;
        h <<= 1;
        y <<= 1;
        if ((h & 0x80000000)) y |= 1;
        h <<= 1;
    }
    return  x;
}

//****************************************************************************
//! Convert Axis Measurement Result Register to mT value
//!
//! Takes in the register of one of the magnetic axis results and the range for
//! that specific axis and calculates the mT value the register represents.
//****************************************************************************
float TMAG5x73resultRegisterTomT(int16_t register_bits, uint16_t range)
{
    return (((float) register_bits) / 32768) * range;
}



//****************************************************************************
//! Convert Angle Result Register to degree value
//!
//! Takes in the bits of the angle result register and converts it to a
//! degree value.
//****************************************************************************
float TMAG5x73angleRegisterToDeg(uint16_t register_bits)
{
    return ((float) register_bits / 16);
}

//****************************************************************************
//! Calculate CRC for I2C data frame
//!
//! i2cRead     - type of I2C Read being used:
//!               0x00 = Standard 3-byte I2C Read
//!               0x01 = 1-byte I2C Read Command for 16-bit Data
//!               0x02 = 1-byte I2C Read Command for 8-bit Data
//!
//! numChannels - number of channels to read, requires a minimum of 1 channel
//!               (NOTE: if using the Standard 3-byte I2C Read, default will be 0x04)
//!               Values if using 1-byte I2C Read Command for 16-/8-bit Data
//!               0x01 = Single Axis Measurement
//!               0x02 = Two Axis Measurement
//!               0x03 = Three Axis Measurement
//!               0x04 = All Sensors Measurement (Not valid for 16-bit Data Read)
//!
//! Takes in an array containing an I2C data frame (MSB to LSB) with the CRC bits
//! all set to ZERO and calculates and returns the CRC for that data frame.
//****************************************************************************
uint8_t TMAG5x73calculateCRC(uint8_t i2cRead, uint8_t numChannels, uint8_t data[])
{
    if ((i2cRead > 0x02) || (numChannels == 0) || (numChannels > 0x04)) assert(1 == 0);

    int i = 0;
    uint8_t crc = 0xFF;
    uint8_t crcNew = 0x00;
    uint8_t d[8];
    uint8_t c[8];
    int j = 0;

    if (i2cRead == 0x01)
        numChannels = numChannels * 2;
    else if (i2cRead == 0x00)
        numChannels = 4;

    for (i = 0; i < numChannels; i++)
    {
        for (j = 0; j < 8; j++)
        {
            d[j] = (data[i] >> j) & 0x01;
            c[j] = (crc >> j) & 0x01;
        }

        crcNew = d[7] ^ d[6] ^ d[0] ^ c[0] ^ c[6] ^ c[7];
        crcNew |= (d[6] ^ d[1] ^ d[0] ^ c[0] ^ c[1] ^ c[6]) << 1;
        crcNew |= (d[6] ^ d[2] ^ d[1] ^ d[0] ^ c[0] ^ c[1] ^ c[2] ^ c[6]) << 2;
        crcNew |= (d[7] ^ d[3] ^ d[2] ^ d[1] ^ c[1] ^ c[2] ^ c[3] ^ c[7]) << 3;
        crcNew |= (d[4] ^ d[3] ^ d[2] ^ c[2] ^ c[3] ^ c[4]) << 4;
        crcNew |= (d[5] ^ d[4] ^ d[3] ^ c[3] ^ c[4] ^ c[5]) << 5;
        crcNew |= (d[6] ^ d[5] ^ d[4] ^ c[4] ^ c[5] ^ c[6]) << 6;
        crcNew |= (d[7] ^ d[6] ^ d[5] ^ c[5] ^ c[6] ^ c[7]) << 7;

        crc = crcNew;

    }

    return crc;
}


//****************************************************************************
//! Verify CRC in I2C data frame
//!
//! i2cRead     - type of I2C Read being used:
//!               0x00 = Standard 3-byte I2C Read
//!               0x01 = 1-byte I2C Read Command for 16-bit Data
//!               0x02 = 1-byte I2C Read Command for 8-bit Data
//!
//! numChannels - number of channels to read, requires a minimum of 1 channel
//!               (NOTE: if using the Standard 3-byte I2C Read, default will be 0x04)
//!               Values if using 1-byte I2C Read Command for 16-/8-bit Data
//!               0x01 = Single Axis Measurement
//!               0x02 = Two Axis Measurement
//!               0x03 = Three Axis Measurement
//!               0x04 = All Sensors Measurement (Not valid for 16-bit Data Read)
//!
//! Takes in an array containing an I2C data frame (MSB to LSB) and checks if the
//! CRC bits (according to their locations for the TMAG5x73) are correct according
//! to the CRC calculation algorithm.
//****************************************************************************
uint8_t TMAG5x73verifyCRC(uint8_t i2cRead, uint8_t numChannels, uint8_t data[])
{
    if ((i2cRead > 0x02) || (numChannels == 0) || (numChannels > 0x04)) assert(1 == 0);

    uint8_t crc_received = data[4] & 0xFF;
    data[4] &= ~(0xFF); // the CRC bits of the data must be 0000b to calculate its CRC correctly
    uint8_t crc_calc = TMAG5x73calculateCRC(i2cRead, numChannels, data);
    data[4] |= crc_received; // the previously removed CRC bits are reinserted

    return crc_received == crc_calc;
}



//****************************************************************************
//! Pulse INT function
//!
//! This function pulses LOW on the GPIO pin connected to the INT pin of the TMAG5x73
//! for 2 us. (set pin to HIGH afterwards)
//!
//! Can be used to trigger conversion for TRIGGER_MODE set to 'at INT pulse'
//!
//! Note: If the INT pin it initialized as an input pin, this function will
//!       not work. INT pin must be initialized as an output pin!
//****************************************************************************
void TMAG5x73intPulse()
{
    setINT(LOW);
    delay_us(2);
    setINT(HIGH);
}
