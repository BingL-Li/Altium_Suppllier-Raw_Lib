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

#include "hal.h"

//****************************************************************************
//
// Global variables
//
//****************************************************************************

// Address_Select is set to the default factory programmed I2C address for the
// TMAG5x73Ax version and may need to be updated if a new I2C address has been
// assigned to the device or if the device has a different default I2C address
//volatile uint8_t    Address_Select      = 0x35; //TODO: Alicia - to be uncommented for final version

//****************************************************************************
//
// Internal variables
//
//****************************************************************************

// Flag to indicate if a /INT interrupt has occurred
static volatile bool flag_nINT_INTERRUPT = false;

//****************************************************************************
//
// Internal function prototypes
//
//****************************************************************************
void InitGPIO(void);
void InitI2C(void);
void GPIO_INT_IRQHandler(void);

void MY_I2CMasterInitExpClk(uint32_t ui32Base, uint32_t ui32I2CClk, bool bFast);

#define I2C_O_MTPR              0x0000000C  // I2C Master Timer Period
#define I2C_O_PP                0x00000FC0  // I2C Peripheral Properties

//****************************************************************************
//
// External Functions (prototypes declared in hal.h)
//
//****************************************************************************

bool getINTinterruptStatus(void)
{
   return flag_nINT_INTERRUPT;
}

void setINTinterruptStatus(const bool value)
{
    flag_nINT_INTERRUPT = value;
}

//*****************************************************************************
//!
//! Initializes MCU peripherals for interfacing with the TMAG5x73.
//!
//! \fn void InitTMAG5x73(void)
//!
//! \return None.
//!
//*****************************************************************************
void InitTMAG5x73(void)
{
    // IMPORTANT: Make sure device is powered before setting GPIOs pins to HIGH state.

    // Initialize GPIOs pins used by EVM
    InitGPIO();

    // Initialize I2C peripheral used by EVM
    InitI2C();

    // Enable timer peripheral
    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);


    // Run TMAG5x73 startup function
    TMAG5x73startup();
}




//****************************************************************************
//
// Timing functions
//
//****************************************************************************



//*****************************************************************************
//!
//! Provides a timing delay with 'ms' resolution.
//!
//! \fn void delay_ms(const uint32_t delay_time_ms)
//!
//! \param delay_time_ms is the number of milliseconds to delay.
//!
//! \return None.
//!
//! NOTE: This function may need to be modified based on the microcontroller
//!       that is being used
//!
//*****************************************************************************
void delay_ms(const uint32_t delay_time_ms)
{
    /* --- INSERT YOUR CODE HERE --- */

    const uint32_t cycles_per_loop = 3;
    MAP_SysCtlDelay( delay_time_ms * getSysClockHz() / (cycles_per_loop * 1000u) );
}



//*****************************************************************************
//!
//! Provides a timing delay with 'us' resolution.
//!
//! \fn void delay_us(const uint32_t delay_time_us)
//!
//! \param delay_time_us is the number of microseconds to delay.
//!
//! \return None.
//!
//! NOTE: This function may need to be modified based on the microcontroller
//!       that is being used
//!
//*****************************************************************************
void delay_us(const uint32_t delay_time_us)
{
    /* --- INSERT YOUR CODE HERE --- */

    const uint32_t cycles_per_loop = 3;
    MAP_SysCtlDelay( delay_time_us * getSysClockHz() / (cycles_per_loop * 1000000u) );
}




//****************************************************************************
//
// GPIO initialization
//
//****************************************************************************



//*****************************************************************************
//!
//! Configures the MCU's GPIO pins that interface with the TMAG5x73.
//!
//! \fn void InitGPIO(void)
//!
//! \return None.
//!
//*****************************************************************************
void InitGPIO(void)
{
    /* --- INSERT YOUR CODE HERE --- */
    // NOTE: Not all hardware implementations may control each of these pins...

    /* Enable the clock to GPIO Ports K and M then wait for it to be ready */
//    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOK);
//    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOM);
//    while(!(SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOK)))
//    {
//    }

    /* Configure the INT pin according to desired functionality */

#ifdef INT_AS_INTERRUPT
    /* Configure the GPIO for 'nINT' as input with falling edge interrupt */
    GPIOIntRegister(nINT_PORT, GPIO_INT_IRQHandler);
    MAP_GPIOPinTypeGPIOInput(nINT_PORT, nINT_PIN);
    MAP_GPIOIntTypeSet(nINT_PORT, nINT_PIN, GPIO_FALLING_EDGE);
    MAP_GPIOIntEnable(nINT_PORT, nINT_PIN);
    MAP_IntEnable(nINT_INT);
#else
    MAP_GPIOPinTypeGPIOOutput(nINT_PORT, nINT_PIN);
    MAP_GPIOPinWrite(nINT_PORT, nINT_PIN, nINT_PIN);
#endif
}



//*****************************************************************************
//
// Interrupt handler for nINT GPIO
//
//*****************************************************************************

//*****************************************************************************
//!
//! Interrupt handler for /INT falling edge interrupt.
//!
//! \fn void GPIO_INT_IRQHandler(void)
//!
//! \return None.
//!
//*****************************************************************************
void GPIO_INT_IRQHandler(void)
{
    /* --- INSERT YOUR CODE HERE --- */
    //NOTE: You many need to rename or register this interrupt function for your processor

    // Possible ways to handle this interrupt:
    // If you decide to read data here, you may want to disable other interrupts to avoid partial data reads.

    // In this example we set a flag and exit the interrupt routine. In the main program loop, your application can examine
    // all state flags and decide which state (operation) to perform next.

    /* Get the interrupt status from the GPIO and clear the status */
    uint32_t getIntStatus = MAP_GPIOIntStatus(nINT_PORT, true);

    /* Check if the nINT pin triggered the interrupt */
    if(getIntStatus & nINT_PIN)
    {
        /* Clear interrupt */
        MAP_GPIOIntClear(nINT_PORT, getIntStatus);

        /* Interrupt action: Set a flag */
        flag_nINT_INTERRUPT = true;
    }

    // NOTE: We add a short delay at the end to prevent re-entrance. Refer to E2E issue:
    // https://e2e.ti.com/support/microcontrollers/tiva_arm/f/908/p/332605/1786938#1786938
    SysCtlDelay(3);
}




//****************************************************************************
//
// GPIO helper functions
//
//****************************************************************************



//*****************************************************************************
//!
//! Reads that current state of the /INT GPIO pin.
//!
//! \fn bool getINT(void)
//!
//! \return boolean ('true' if /INT is high, 'false' if /INT is low).
//!
//*****************************************************************************
bool getINT(void)
{
    /* --- INSERT YOUR CODE HERE --- */
    return (bool) GPIOPinRead(nINT_PORT, nINT_PIN);
}



//*****************************************************************************
//!
//! Controls the state of the /INT GPIO pin.
//!
//! \fn void setINT(const bool state)
//!
//! \param state boolean indicating which state to set the /INT pin (0=low, 1=high)
//!
//! NOTE: The 'HIGH' and 'LOW' macros defined in hal.h can be passed to this
//! function for the 'state' parameter value.
//!
//! Note: If the INT pin it initialized as an input pin, this function will
//!       not work. INT pin must be initialized as an output pin!
//!
//! \return None.
//!
//*****************************************************************************
void setINT(const bool state)
{
    /* --- INSERT YOUR CODE HERE --- */

    // td(CSSC) delay
    if(state) { SysCtlDelay(2); }

    uint8_t value = (uint8_t) (state ? nINT_PIN : 0);
    MAP_GPIOPinWrite(nINT_PORT, nINT_PIN, value);

    // td(SCCS) delay
    if(!state) { SysCtlDelay(2); }
}


//*****************************************************************************
//!
//! Waits for the nINT interrupt or until the specified timeout occurs.
//!
//! \fn bool waitForINTinterrupt(const uint32_t timeout_ms)
//!
//! \param timeout_ms number of milliseconds to wait before timeout event.
//!
//! \return Returns 'true' if nINT interrupt occurred before the timeout.
//!
//*****************************************************************************
bool waitForINTinterrupt(const uint32_t timeout_ms)
{
    /* --- INSERT YOUR CODE HERE ---
     * Poll the nINT GPIO pin until it goes low. To avoid potential infinite
     * loops, you may also want to implement a timer interrupt to occur after
     * the specified timeout period, in case the nINT pin is not active.
     * Return a boolean to indicate if nINT went low or if a timeout occurred.
     */

    uint32_t timeout = timeout_ms * 6000;   // convert to # of loop iterations

    // Reset interrupt flag
    flag_nINT_INTERRUPT = false;

    // Enable interrupts
    IntMasterEnable();

    // Wait for nINT interrupt or timeout - each iteration is about 20 ticks
    do {
        timeout--;
    } while (!flag_nINT_INTERRUPT && (timeout > 0));

    // Reset interrupt flag
    flag_nINT_INTERRUPT = false;

    // Timeout counter greater than zero indicates that an interrupt occurred
    return (timeout > 0);
}

//****************************************************************************
//
// I2C Communication
//
//****************************************************************************


//*****************************************************************************
//!
//! Configures the MCU's I2C peripheral, for interfacing with the TMAG5x73.
//!
//! \fn void InitI2C(void)
//!
//! \return None.
//!
//*****************************************************************************
void InitI2C(void)
{
    //I2C
    /* Enable GPIO for Configuring the I2C Interface Pins*/
    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);

    /* Wait for the Peripheral to be ready for programming*/
    while(!MAP_SysCtlPeripheralReady(SYSCTL_PERIPH_GPION));

    /* Configure Pins for I2C2 Master Interface*/
    MAP_GPIOPinConfigure(GPIO_PN5_I2C2SCL);
    MAP_GPIOPinConfigure(GPIO_PN4_I2C2SDA);

    MAP_GPIOPinTypeI2C(GPIO_PORTN_BASE, GPIO_PIN_4);
    MAP_GPIOPinTypeI2CSCL(GPIO_PORTN_BASE, GPIO_PIN_5);

    GPION->PUR |= (GPIO_PIN_4 | GPIO_PIN_5); //We do have a board pull-up

    /* Stop the Clock, Reset and Enable I2C Module in Master Function */
    MAP_SysCtlPeripheralDisable(SYSCTL_PERIPH_I2C2);
    MAP_SysCtlPeripheralReset(SYSCTL_PERIPH_I2C2);
    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C2);

    /* Wait for the Peripheral to be ready for programming */
    while(!MAP_SysCtlPeripheralReady(SYSCTL_PERIPH_I2C2));

#ifdef I2C_CLOCK_SPEED_100kHz         //APW changed to normal speed
    APW_Normal_Speed_400kHz = false;  // false - 100kHz
#else
    APW_Normal_Speed_400kHz = true;   // true - 400kHz
#endif

    /* Initialize and Configure the Master Module */
    MY_I2CMasterInitExpClk(I2C2_BASE, getSysClockHz(), APW_Normal_Speed_400kHz);

    /* Check if the Bus is Busy or not */
    while(MAP_I2CMasterBusBusy(I2C2_BASE));
}

//*****************************************************************************
//!
//! \fn void i2cSendArrays(const uint8_t dataTx[], const uint8_t byteLength)
//!
//! \param const uint8_t dataTx[] byte array of I2C data to send on SDA.
//!
//! \param uint8_t byteLength number of bytes to send.
//!
//! NOTE: Make sure 'dataTx[]' contains at least as many bytes of data,
//! as indicated by 'byteLength'.
//!
//! \return None.
//!
//*****************************************************************************
void i2cSendArrays(const uint8_t dataTx[], const uint8_t byteLength)
{

    int i;

    //Set slave address with write bit
    MAP_I2CMasterSlaveAddrSet(I2C2_BASE, Address_Select, false);

    if (byteLength > 1){
        //Send first byte
        MAP_I2CMasterDataPut(I2C2_BASE, dataTx[0]);
        MAP_I2CMasterControl(I2C2_BASE, I2C_MASTER_CMD_BURST_SEND_START);
        while(MAP_I2CMasterBusy(I2C2_BASE)){} // Delay until transmission completes
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
        while(MAP_I2CMasterBusy(I2C2_BASE)){}

#ifdef I2C_CLOCK_SPEED_100kHz
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
#endif

        //Send remaining bytes
        for (i = 1; i < byteLength-1; i++)
        {
            MAP_I2CMasterDataPut(I2C2_BASE, dataTx[i]);
            MAP_I2CMasterControl(I2C2_BASE, I2C_MASTER_CMD_BURST_SEND_CONT);
            while(MAP_I2CMasterBusy(I2C2_BASE)){} // Delay until transmission completes
            while(MAP_I2CMasterBusy(I2C2_BASE)){}
            while(MAP_I2CMasterBusy(I2C2_BASE)){}
            while(MAP_I2CMasterBusy(I2C2_BASE)){}

#ifdef I2C_CLOCK_SPEED_100kHz
            while(MAP_I2CMasterBusy(I2C2_BASE)){}
            while(MAP_I2CMasterBusy(I2C2_BASE)){}
            while(MAP_I2CMasterBusy(I2C2_BASE)){}
            while(MAP_I2CMasterBusy(I2C2_BASE)){}
            while(MAP_I2CMasterBusy(I2C2_BASE)){}
            while(MAP_I2CMasterBusy(I2C2_BASE)){}
            while(MAP_I2CMasterBusy(I2C2_BASE)){}
            while(MAP_I2CMasterBusy(I2C2_BASE)){}
            while(MAP_I2CMasterBusy(I2C2_BASE)){}
            while(MAP_I2CMasterBusy(I2C2_BASE)){}
            while(MAP_I2CMasterBusy(I2C2_BASE)){}
            while(MAP_I2CMasterBusy(I2C2_BASE)){}
            while(MAP_I2CMasterBusy(I2C2_BASE)){}
            while(MAP_I2CMasterBusy(I2C2_BASE)){}
            while(MAP_I2CMasterBusy(I2C2_BASE)){}
            while(MAP_I2CMasterBusy(I2C2_BASE)){}
            while(MAP_I2CMasterBusy(I2C2_BASE)){}
            while(MAP_I2CMasterBusy(I2C2_BASE)){}
            while(MAP_I2CMasterBusy(I2C2_BASE)){}
            while(MAP_I2CMasterBusy(I2C2_BASE)){}
            while(MAP_I2CMasterBusy(I2C2_BASE)){}
            while(MAP_I2CMasterBusy(I2C2_BASE)){}
            while(MAP_I2CMasterBusy(I2C2_BASE)){}
#endif

        }
        MAP_I2CMasterDataPut(I2C2_BASE, dataTx[byteLength-1]);
        MAP_I2CMasterControl(I2C2_BASE, I2C_MASTER_CMD_BURST_SEND_FINISH);
        while(MAP_I2CMasterBusy(I2C2_BASE)){} // Delay until transmission completes
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
        while(MAP_I2CMasterBusy(I2C2_BASE)){}

#ifdef I2C_CLOCK_SPEED_100kHz
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
#endif

    }
    else{
        MAP_I2CMasterDataPut(I2C2_BASE, dataTx[0]);
        MAP_I2CMasterControl(I2C2_BASE, I2C_MASTER_CMD_SINGLE_SEND);
        while(MAP_I2CMasterBusy(I2C2_BASE)){} // Delay until transmission completes
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
        while(MAP_I2CMasterBusy(I2C2_BASE)){}

#ifdef I2C_CLOCK_SPEED_100kHz
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
#endif
    }
}

//*****************************************************************************
//!
//! \fn void i2cReceiveArrays(uint8_t dataRx[], const uint8_t byteLength)
//!
//! \param uint8_t dataRx[] byte array of I2C data captured on SDA.
//!
//! \param uint8_t byteLength number of bytes to receive.
//!
//! NOTE: Make sure 'dataRx[]' contains at least as large as the number of bytes of data,
//! as indicated by 'byteLength'.
//!
//! \return None.
//!
//*****************************************************************************
void i2cReceiveArrays(uint8_t dataRx[], const uint8_t byteLength, uint8_t dataTx[])
{

    int i;

    //Set slave address with write bit
    MAP_I2CMasterSlaveAddrSet(I2C2_BASE, Address_Select, false); //(master address function, slave address, direction - t-read f-write)

    //Send first byte
   MAP_I2CMasterDataPut(I2C2_BASE, dataTx[0]); //Sends a singly byte (4.4 Data Transfer Commands for I2C Master in MSP432E4)

    //MAP_I2CMasterControl(I2C2_BASE, I2C_MASTER_CMD_SINGLE_SEND);
   MAP_I2CMasterControl(I2C2_BASE, I2C_MASTER_CMD_BURST_SEND_START);

   // MAP_I2CMasterControl(I2C2_BASE, I2C_MASTER_CMD_SINGLE_SEND); //setup write to [0x30] + Ack...0x18 + Ack
    while(MAP_I2CMasterBusy(I2C2_BASE)){} // Delay until transmission completes
    while(MAP_I2CMasterBusy(I2C2_BASE)){}
    while(MAP_I2CMasterBusy(I2C2_BASE)){}
    while(MAP_I2CMasterBusy(I2C2_BASE)){}
    while(MAP_I2CMasterBusy(I2C2_BASE)){}

#ifdef I2C_CLOCK_SPEED_100kHz
    while(MAP_I2CMasterBusy(I2C2_BASE)){}
    while(MAP_I2CMasterBusy(I2C2_BASE)){}
    while(MAP_I2CMasterBusy(I2C2_BASE)){}
    while(MAP_I2CMasterBusy(I2C2_BASE)){}
    while(MAP_I2CMasterBusy(I2C2_BASE)){}
    while(MAP_I2CMasterBusy(I2C2_BASE)){}
    while(MAP_I2CMasterBusy(I2C2_BASE)){}
    while(MAP_I2CMasterBusy(I2C2_BASE)){}
    while(MAP_I2CMasterBusy(I2C2_BASE)){}
    while(MAP_I2CMasterBusy(I2C2_BASE)){}
    while(MAP_I2CMasterBusy(I2C2_BASE)){}
    while(MAP_I2CMasterBusy(I2C2_BASE)){}
    while(MAP_I2CMasterBusy(I2C2_BASE)){}
    while(MAP_I2CMasterBusy(I2C2_BASE)){}
    while(MAP_I2CMasterBusy(I2C2_BASE)){}
    while(MAP_I2CMasterBusy(I2C2_BASE)){}
    while(MAP_I2CMasterBusy(I2C2_BASE)){}
    while(MAP_I2CMasterBusy(I2C2_BASE)){}
    while(MAP_I2CMasterBusy(I2C2_BASE)){}
    while(MAP_I2CMasterBusy(I2C2_BASE)){}
    while(MAP_I2CMasterBusy(I2C2_BASE)){}
    while(MAP_I2CMasterBusy(I2C2_BASE)){}
    while(MAP_I2CMasterBusy(I2C2_BASE)){}
    while(MAP_I2CMasterBusy(I2C2_BASE)){}
    while(MAP_I2CMasterBusy(I2C2_BASE)){}
    while(MAP_I2CMasterBusy(I2C2_BASE)){}
#endif

    //Set slave address with read bit
    MAP_I2CMasterSlaveAddrSet(I2C2_BASE, Address_Select, true);

    if (byteLength > 1){
        //Read first byte
        MAP_I2CMasterSlaveAddrSet(I2C2_BASE, Address_Select, true);
        MAP_I2CMasterControl(I2C2_BASE, I2C_MASTER_CMD_BURST_RECEIVE_START);
        while(MAP_I2CMasterBusy(I2C2_BASE)){} // Delay until transmission completes
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
        while(MAP_I2CMasterBusy(I2C2_BASE)){}

#ifdef I2C_CLOCK_SPEED_100kHz
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
#endif

        dataRx[0] = MAP_I2CMasterDataGet(I2C2_BASE);

        //Read remaining bytes
        for (i = 1; i < byteLength-1; i++)
        {
            MAP_I2CMasterControl(I2C2_BASE, I2C_MASTER_CMD_BURST_RECEIVE_CONT);
            while(MAP_I2CMasterBusy(I2C2_BASE)){} // Delay until transmission completes
            while(MAP_I2CMasterBusy(I2C2_BASE)){}
            while(MAP_I2CMasterBusy(I2C2_BASE)){}
            while(MAP_I2CMasterBusy(I2C2_BASE)){}
            while(MAP_I2CMasterBusy(I2C2_BASE)){}

#ifdef I2C_CLOCK_SPEED_100kHz
            while(MAP_I2CMasterBusy(I2C2_BASE)){}
            while(MAP_I2CMasterBusy(I2C2_BASE)){}
            while(MAP_I2CMasterBusy(I2C2_BASE)){}
            while(MAP_I2CMasterBusy(I2C2_BASE)){}
            while(MAP_I2CMasterBusy(I2C2_BASE)){}
            while(MAP_I2CMasterBusy(I2C2_BASE)){}
            while(MAP_I2CMasterBusy(I2C2_BASE)){}
            while(MAP_I2CMasterBusy(I2C2_BASE)){}
            while(MAP_I2CMasterBusy(I2C2_BASE)){}
            while(MAP_I2CMasterBusy(I2C2_BASE)){}
            while(MAP_I2CMasterBusy(I2C2_BASE)){}
            while(MAP_I2CMasterBusy(I2C2_BASE)){}
            while(MAP_I2CMasterBusy(I2C2_BASE)){}
            while(MAP_I2CMasterBusy(I2C2_BASE)){}
            while(MAP_I2CMasterBusy(I2C2_BASE)){}
            while(MAP_I2CMasterBusy(I2C2_BASE)){}
            while(MAP_I2CMasterBusy(I2C2_BASE)){}
            while(MAP_I2CMasterBusy(I2C2_BASE)){}
            while(MAP_I2CMasterBusy(I2C2_BASE)){}
            while(MAP_I2CMasterBusy(I2C2_BASE)){}
            while(MAP_I2CMasterBusy(I2C2_BASE)){}
            while(MAP_I2CMasterBusy(I2C2_BASE)){}
            while(MAP_I2CMasterBusy(I2C2_BASE)){}
            while(MAP_I2CMasterBusy(I2C2_BASE)){}
            while(MAP_I2CMasterBusy(I2C2_BASE)){}
            while(MAP_I2CMasterBusy(I2C2_BASE)){}
#endif

            dataRx[i] = MAP_I2CMasterDataGet(I2C2_BASE);
        }
            MAP_I2CMasterControl(I2C2_BASE, I2C_MASTER_CMD_BURST_RECEIVE_FINISH);
            while(MAP_I2CMasterBusy(I2C2_BASE)){} // Delay until transmission completes
            while(MAP_I2CMasterBusy(I2C2_BASE)){}
            while(MAP_I2CMasterBusy(I2C2_BASE)){}
            while(MAP_I2CMasterBusy(I2C2_BASE)){}
            while(MAP_I2CMasterBusy(I2C2_BASE)){}

#ifdef I2C_CLOCK_SPEED_100kHz
            while(MAP_I2CMasterBusy(I2C2_BASE)){}
            while(MAP_I2CMasterBusy(I2C2_BASE)){}
            while(MAP_I2CMasterBusy(I2C2_BASE)){}
            while(MAP_I2CMasterBusy(I2C2_BASE)){}
            while(MAP_I2CMasterBusy(I2C2_BASE)){}
            while(MAP_I2CMasterBusy(I2C2_BASE)){}
            while(MAP_I2CMasterBusy(I2C2_BASE)){}
            while(MAP_I2CMasterBusy(I2C2_BASE)){}
            while(MAP_I2CMasterBusy(I2C2_BASE)){}
            while(MAP_I2CMasterBusy(I2C2_BASE)){}
            while(MAP_I2CMasterBusy(I2C2_BASE)){}
            while(MAP_I2CMasterBusy(I2C2_BASE)){}
            while(MAP_I2CMasterBusy(I2C2_BASE)){}
            while(MAP_I2CMasterBusy(I2C2_BASE)){}
            while(MAP_I2CMasterBusy(I2C2_BASE)){}
            while(MAP_I2CMasterBusy(I2C2_BASE)){}
            while(MAP_I2CMasterBusy(I2C2_BASE)){}
            while(MAP_I2CMasterBusy(I2C2_BASE)){}
            while(MAP_I2CMasterBusy(I2C2_BASE)){}
            while(MAP_I2CMasterBusy(I2C2_BASE)){}
            while(MAP_I2CMasterBusy(I2C2_BASE)){}
            while(MAP_I2CMasterBusy(I2C2_BASE)){}
            while(MAP_I2CMasterBusy(I2C2_BASE)){}
            while(MAP_I2CMasterBusy(I2C2_BASE)){}
            while(MAP_I2CMasterBusy(I2C2_BASE)){}
            while(MAP_I2CMasterBusy(I2C2_BASE)){}
#endif

            dataRx[byteLength-1] = MAP_I2CMasterDataGet(I2C2_BASE);
    }
    else{
        MAP_I2CMasterSlaveAddrSet(I2C2_BASE, Address_Select, true);
        MAP_I2CMasterControl(I2C2_BASE, I2C_MASTER_CMD_SINGLE_RECEIVE);
        while(MAP_I2CMasterBusy(I2C2_BASE)){} // Delay until transmission completes
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
        while(MAP_I2CMasterBusy(I2C2_BASE)){}

#ifdef I2C_CLOCK_SPEED_100kHz
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
#endif

        dataRx[0] = MAP_I2CMasterDataGet(I2C2_BASE);
    }
}

//*****************************************************************************
//!
//! \fn void i2cOneByteReceiveArrays(uint8_t dataRx[], const uint8_t byteLength)
//!
//! Used for when the I2C_RD is set to use a 1-byte I2C read command
//!
//! \param uint8_t dataRx[] byte array of I2C data captured on SDA.
//!
//! \param uint8_t byteLength number of bytes to receive.
//!
//! NOTE: Make sure 'dataRx[]' contains at least as large as the number of bytes of data,
//! as indicated by 'byteLength'.
//!
//! \return None.
//!
//*****************************************************************************
void i2cOneByteReceiveArrays(uint8_t dataRx[], const uint8_t byteLength)
{

    int i;

    //Set slave address with read bit
    MAP_I2CMasterSlaveAddrSet(I2C2_BASE, Address_Select, true);

    if (byteLength > 1){
        //Read first byte
        MAP_I2CMasterSlaveAddrSet(I2C2_BASE, Address_Select, true);
        MAP_I2CMasterControl(I2C2_BASE, I2C_MASTER_CMD_BURST_RECEIVE_START);
        while(MAP_I2CMasterBusy(I2C2_BASE)){} // Delay until transmission completes
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
        while(MAP_I2CMasterBusy(I2C2_BASE)){}

#ifdef I2C_CLOCK_SPEED_100kHz
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
#endif

        dataRx[0] = MAP_I2CMasterDataGet(I2C2_BASE);

        //Read remaining bytes
        for (i = 1; i < byteLength-1; i++)
        {
            MAP_I2CMasterControl(I2C2_BASE, I2C_MASTER_CMD_BURST_RECEIVE_CONT);
            while(MAP_I2CMasterBusy(I2C2_BASE)){} // Delay until transmission completes
            while(MAP_I2CMasterBusy(I2C2_BASE)){}
            while(MAP_I2CMasterBusy(I2C2_BASE)){}
            while(MAP_I2CMasterBusy(I2C2_BASE)){}

#ifdef I2C_CLOCK_SPEED_100kHz
            while(MAP_I2CMasterBusy(I2C2_BASE)){}
            while(MAP_I2CMasterBusy(I2C2_BASE)){}
            while(MAP_I2CMasterBusy(I2C2_BASE)){}
            while(MAP_I2CMasterBusy(I2C2_BASE)){}
            while(MAP_I2CMasterBusy(I2C2_BASE)){}
            while(MAP_I2CMasterBusy(I2C2_BASE)){}
            while(MAP_I2CMasterBusy(I2C2_BASE)){}
            while(MAP_I2CMasterBusy(I2C2_BASE)){}
            while(MAP_I2CMasterBusy(I2C2_BASE)){}
            while(MAP_I2CMasterBusy(I2C2_BASE)){}
            while(MAP_I2CMasterBusy(I2C2_BASE)){}
            while(MAP_I2CMasterBusy(I2C2_BASE)){}
            while(MAP_I2CMasterBusy(I2C2_BASE)){}
            while(MAP_I2CMasterBusy(I2C2_BASE)){}
            while(MAP_I2CMasterBusy(I2C2_BASE)){}
            while(MAP_I2CMasterBusy(I2C2_BASE)){}
            while(MAP_I2CMasterBusy(I2C2_BASE)){}
            while(MAP_I2CMasterBusy(I2C2_BASE)){}
            while(MAP_I2CMasterBusy(I2C2_BASE)){}
            while(MAP_I2CMasterBusy(I2C2_BASE)){}
            while(MAP_I2CMasterBusy(I2C2_BASE)){}
            while(MAP_I2CMasterBusy(I2C2_BASE)){}
            while(MAP_I2CMasterBusy(I2C2_BASE)){}
            while(MAP_I2CMasterBusy(I2C2_BASE)){}
            while(MAP_I2CMasterBusy(I2C2_BASE)){}
            while(MAP_I2CMasterBusy(I2C2_BASE)){}
#endif

            dataRx[i] = MAP_I2CMasterDataGet(I2C2_BASE);
        }
            MAP_I2CMasterControl(I2C2_BASE, I2C_MASTER_CMD_BURST_RECEIVE_FINISH);
            while(MAP_I2CMasterBusy(I2C2_BASE)){} // Delay until transmission completes
            while(MAP_I2CMasterBusy(I2C2_BASE)){}
            while(MAP_I2CMasterBusy(I2C2_BASE)){}
            while(MAP_I2CMasterBusy(I2C2_BASE)){}

#ifdef I2C_CLOCK_SPEED_100kHz
            while(MAP_I2CMasterBusy(I2C2_BASE)){}
            while(MAP_I2CMasterBusy(I2C2_BASE)){}
            while(MAP_I2CMasterBusy(I2C2_BASE)){}
            while(MAP_I2CMasterBusy(I2C2_BASE)){}
            while(MAP_I2CMasterBusy(I2C2_BASE)){}
            while(MAP_I2CMasterBusy(I2C2_BASE)){}
            while(MAP_I2CMasterBusy(I2C2_BASE)){}
            while(MAP_I2CMasterBusy(I2C2_BASE)){}
            while(MAP_I2CMasterBusy(I2C2_BASE)){}
            while(MAP_I2CMasterBusy(I2C2_BASE)){}
            while(MAP_I2CMasterBusy(I2C2_BASE)){}
            while(MAP_I2CMasterBusy(I2C2_BASE)){}
            while(MAP_I2CMasterBusy(I2C2_BASE)){}
            while(MAP_I2CMasterBusy(I2C2_BASE)){}
            while(MAP_I2CMasterBusy(I2C2_BASE)){}
            while(MAP_I2CMasterBusy(I2C2_BASE)){}
            while(MAP_I2CMasterBusy(I2C2_BASE)){}
            while(MAP_I2CMasterBusy(I2C2_BASE)){}
            while(MAP_I2CMasterBusy(I2C2_BASE)){}
            while(MAP_I2CMasterBusy(I2C2_BASE)){}
            while(MAP_I2CMasterBusy(I2C2_BASE)){}
            while(MAP_I2CMasterBusy(I2C2_BASE)){}
            while(MAP_I2CMasterBusy(I2C2_BASE)){}
            while(MAP_I2CMasterBusy(I2C2_BASE)){}
            while(MAP_I2CMasterBusy(I2C2_BASE)){}
            while(MAP_I2CMasterBusy(I2C2_BASE)){}
#endif

            dataRx[byteLength-1] = MAP_I2CMasterDataGet(I2C2_BASE);
    }
    else{
        MAP_I2CMasterSlaveAddrSet(I2C2_BASE, Address_Select, true);
        MAP_I2CMasterControl(I2C2_BASE, I2C_MASTER_CMD_SINGLE_RECEIVE);
        while(MAP_I2CMasterBusy(I2C2_BASE)){} // Delay until transmission completes
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
        while(MAP_I2CMasterBusy(I2C2_BASE)){}

#ifdef I2C_CLOCK_SPEED_100kHz
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
        while(MAP_I2CMasterBusy(I2C2_BASE)){}
#endif

        dataRx[0] = MAP_I2CMasterDataGet(I2C2_BASE);
    }
}



//*****************************************************************************
//!
//! Overwrite I2C init function to choose custom HS frequency
//!
//*****************************************************************************
void MY_I2CMasterInitExpClk(uint32_t ui32Base, uint32_t ui32I2CClk, bool bFast)
{
    uint32_t ui32SCLFreq;
    uint32_t ui32TPR;

    //
    // Check the arguments.
    //
    ASSERT(_I2CBaseValid(ui32Base));

    //
    // Must enable the device before doing anything else.
    //
    I2CMasterEnable(ui32Base);

    //
    // Get the desired SCL speed.
    //
    if(bFast == true)
    {
        ui32SCLFreq = 400000;
    }
    else
    {
        ui32SCLFreq = 100000;
    }

    //
    // Compute the clock divider that achieves the fastest speed less than or
    // equal to the desired speed.  The numerator is biased to favor a larger
    // clock divider so that the resulting clock is always less than or equal
    // to the desired clock, never greater.
    //
    ui32TPR = ((ui32I2CClk + (2 * 10 * ui32SCLFreq) - 1) /
               (2 * 10 * ui32SCLFreq)) - 1;
    HWREG(ui32Base + I2C_O_MTPR) = ui32TPR;

    //
    // Check to see if this I2C peripheral is High-Speed enabled.  If yes, also
    // choose the fastest speed that is less than or equal to 2.94Mbps // was 3.4 Mbps.
    //
    if(HWREG(ui32Base + I2C_O_PP) & I2C_PP_HS)
    {
        ui32TPR = ((ui32I2CClk + (2 * 3 * 2940000) - 1) /
                   (2 * 3 * 2940000)) - 1;
        HWREG(ui32Base + I2C_O_MTPR) = I2C_MTPR_HS | ui32TPR;
    }
}
