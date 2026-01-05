![IC Image](https://www.ti.com/graphics/folders/partimages/TMAG5273.jpg)TMAG5x73 Example Code
=====================

The TMAG5x73 low-power linear 3D Hall-effect sensor communicates using an I2C interface to allow for device configuration, control, and data retrieval. To interface the TMAG5x73 device with a microcontroller (MCU), the firmware or software engineer needs to know how to correctly configure their MCU. To assist in expediting this process, this example code is intended to show how to initialize and communicate with the TMAG5x73 at a high-level.

Included modules
----------------

There are three modules included in this example code:

1.  **`TMAG5x73`**

	*Description:* High-level functions for communicating with and using the functions of the TMAG5x73.
	
	*Files: tmag5x73.h, tmag5x73_ex.c*

2.  **`Hardware Abstraction Layer (HAL)`**

	*Description:* The HAL provides processor specific functions called by the `TMAG5x73` module.
	
	*Files: hal.h, hal.c*
	
	**IMPORTANT**: This module will need to be modified to work with your hardware!


How to use this code
--------------------

Reference the *tmag5x73_ex.c* file while writing your own code for examples of how to read/write registers and implement the different capabilities of the sensor...

OR

Copy and paste the example code into your project, and update the files as needed to get access to the provided APIs...

 1. Copy the `TMAG5x73` and `HAL` module files into your firmware project.
 2. Add library references in *hal.h* to your processor-specific library file(s).
	```c
	//****************************************************************************
	//
	// Insert processor specific header file(s) here
	//
	//****************************************************************************"
	
	/*  --- TODO: INSERT YOUR CODE HERE --- */
	#include "ti/devices/msp432e4/driverlib/driverlib.h"
	
	```

 3. Edit all of the function implementations inside of *hal.c* to provide the specified functionality with your processor and processor-specific library APIs. 
	```c
	//*****************************************************************************
	//
	//! Configures the MCU's I2C peripheral, for interfacing with the TMAG5x73.
	//!
	//! \fn void InitI2C(void)
	//!
	//! \return None.
	//
	//*****************************************************************************
	void InitI2C(void)
	{
		/* --- INSERT YOUR CODE HERE ---
		* 
		*/
	```
	NOTE 1: The *hal.c* functions are called from within *tmag5x73_ex.c* to interface with the hardware. The hardware functionality is kept in a separate module to allow the `TMAG5x73` module to remain portable.
	
	NOTE 2: Provided code examples in *hal.c* functions utilize the [TI SimpleLink SDK](http://www.ti.com/wireless-connectivity/simplelink-solutions/overview/software.html) and allow for quick integration with TI SimpleLink MCUs.
	
 4. Include a reference to *hal.h* somewhere in your program and call the InitTMAG5x73()** function to initialize the MCU peripherals connected to the TMAG5x73.

 5. Include a reference to *tmag5x73.h* in your application (from *main.c* or wherever sensor communication is handled). You should now be able to begin calling the `TMAG5x73` module functions in your code.

> **DISCLAIMER**: This code was tested on an MSP432E401Y 32-bit ARM® Cortex®-M4F based MCU using TI Code Composer Studio's 20.2.6.LTS ARM compiler. This code is provided as example to aid in the creation of your own software implementation and should not be considered to be fully verified and production ready. This example code was written for readability and has not been optimized for performance.

Support
-------

For questions or issues, visit the [TI E2E Forums](https://e2e.ti.com/).


Release History
---------------
| Version     | Date        | Description            |
|:-----------:| ----------- | ---------------------- |
| 1.0.0       | 12/6/2022   | Initial release        |

License
-------

**BSD-3-Clause**  
Refer to *manifest.html* for license text...

![TI Logo](http://www.ti.com/assets/images/ic-logo.png)  
*Copyright (C) 2022 Texas Instruments Incorporated - http://www.ti.com/*