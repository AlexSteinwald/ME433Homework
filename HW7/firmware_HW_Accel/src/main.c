/*******************************************************************************
  MPLAB Harmony Project Main Source File

  Company:
    Microchip Technology Inc.
  
  File Name:
    main.c

  Summary:
    This file contains the "main" function for an MPLAB Harmony project.

  Description:
    This file contains the "main" function for an MPLAB Harmony project.  The
    "main" function calls the "SYS_Initialize" function to initialize the state 
    machines of all MPLAB Harmony modules in the system and it calls the 
    "SYS_Tasks" function from within a system-wide "super" loop to maintain 
    their correct operation. These two functions are implemented in 
    configuration-specific files (usually "system_init.c" and "system_tasks.c")
    in a configuration-specific folder under the "src/system_config" folder 
    within this project's top-level folder.  An MPLAB Harmony project may have
    more than one configuration, each contained within it's own folder under
    the "system_config" folder.
 *******************************************************************************/

// DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2013-2014 released Microchip Technology Inc.  All rights reserved.

//Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
 *******************************************************************************/
// DOM-IGNORE-END


// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include <stddef.h>                     // Defines NULL
#include <stdbool.h>                    // Defines true
#include <stdlib.h>                     // Defines EXIT_FAILURE
#include "system/common/sys_module.h"   // SYS function prototypes

//For Accel:
#include<xc.h> // processor SFR definitions
#include<sys/attribs.h> // __ISR macro
#include "accel.h"
#include "i2c_display.h"
#include "i2c_master_int.h"


// *****************************************************************************
// *****************************************************************************
// Section: Main Entry Point
// *****************************************************************************
// *****************************************************************************

int main ( void )
{
    /* Initialize all MPLAB Harmony modules, including application(s). */
    SYS_Initialize ( NULL );

    display_init(); //Setup LED Display
    acc_setup(); //Setup accelerometer
    short accels[3]; // accelerations for the 3 axes
    short mags[3]; // magnetometer readings for the 3 axes
    short temp; // temp reading

    while ( true )
    {
        /* Maintain state machines of all polled MPLAB Harmony modules. */
        SYS_Tasks ( );

        //START ACCEL FUNCTION
                display_clear(); //Clear existing LED bit settings

        //        BEGIN HW5 CODE

        // read the accelerometer from all three axes
        // the accelerometer and the pic32 are both little endian by default (the lowest address has the LSB)
        // the accelerations are 16-bit twos compliment numbers, the same as a short
        acc_read_register(OUT_X_L_A, (unsigned char *) accels, 6);

        // need to read all 6 bytes in one transaction to get an update.
        acc_read_register(OUT_X_L_M, (unsigned char *) mags, 6);

        // read the temperature data. Its a right justified 12 bit two's compliment number
        acc_read_register(TEMP_OUT_L, (unsigned char *) &temp, 2);

        int xLen = accels[0] / 250; // Turns raw x data into +/- 64
        int yLen = accels[1] / 500; // Turns raw y data into +/- 32
        int ix1;
        int ix2;
        int iy1;
        int iy2;

        if (xLen > 0) { //If in positive X direction
            for (ix1 = 0; ix1 < xLen; ix1++) { //Go along and set bit on for that length
                display_pixel_set(32, 64 - ix1, 1); //Starting in midde
            }
        }
        if (xLen < 0) {
            for (ix2 = 0; ix2 > xLen; ix2--) {
                display_pixel_set(32, 64 - ix2, 1);
            }
        }
        if (yLen > 0) {
            for (iy1 = 0; iy1 < yLen; iy1++) {
                display_pixel_set(32 - iy1, 64, 1);
            }
        }
        if (yLen < 0) {
            for (iy2 = 0; iy2 > yLen; iy2--) {
                display_pixel_set(32 - iy2, 64, 1);
            }
        }
        //        END HW5 CODE

        display_draw(); //Commit to LCD screen once all positions defined
        //END ACCEL FUNCTION

    }

    /* Execution should not come here during normal operation */

    return ( EXIT_FAILURE );
}


/*******************************************************************************
 End of File
*/