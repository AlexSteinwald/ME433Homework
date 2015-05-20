/*******************************************************
 Windows HID simplification

 Alan Ott
 Signal 11 Software

 8/22/2009

 Copyright 2009
 
 This contents of this file may be used by anyone
 for any reason without any conditions and may be
 used as a starting point for your own applications
 which use HIDAPI.
********************************************************/

#ifdef WIN32
#include <windows.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include "hidapi.h"

#define MAX_STR 255

int main(int argc, char* argv[])
{
	int res;
	unsigned char buf[65];
	wchar_t wstr[MAX_STR];
	hid_device *handle;
	int i;

	// Initialize the hidapi library
	res = hid_init();

	// Open the device using the VID, PID,
	// and optionally the Serial number.
	handle = hid_open(0x4d8, 0x3f, NULL);

	// Read the Manufacturer String
	res = hid_get_manufacturer_string(handle, wstr, MAX_STR);
	wprintf(L"Manufacturer String: %s\n", wstr);

	// Read the Product String
	res = hid_get_product_string(handle, wstr, MAX_STR);
	wprintf(L"Product String: %s\n", wstr);

	// Read the Serial Number String
	res = hid_get_serial_number_string(handle, wstr, MAX_STR);
	wprintf(L"Serial Number String: (%d) %s\n", wstr[0], wstr);

	// Read Indexed String 1
	res = hid_get_indexed_string(handle, 1, wstr, MAX_STR);
	wprintf(L"Indexed String 1: %s\n", wstr);

	// Toggle LED (cmd 0x80). The first byte is the report number (0x0).
	buf[0] = 0x0;
	buf[1] = 0x80;

        int RowNum;
		printf("Input Row Number:   ");
        scanf("%d", &RowNum); //Ask user for row number
        buf[2] = RowNum; //Commit row number input in 2nd spot
		
		char MessageInput[40];
		printf("Input Message (max 40 characters):  ");
		scanf("%s", &MessageInput);//Ask user for message and make it a string
		
		int StringCount;
		for (StringCount = 0; StringCount<40; StringCount++) {
				buf[3+StringCount] = MessageInput[StringCount];//Commit message to spot on array starting at buf[3]
}
	res = hid_write(handle, buf, 65);
	
	// Request state (cmd 0x81). The first byte is the report number (0x0).
	int numReads = 50;
	short AccelsTable[numReads][3];
	
	for (int countReq = 0; countReq < numReads; countReq++){
	
	buf[0] = 0x0;
	buf[1] = 0x81;
	res = hid_write(handle, buf, 65);

	// Read requested state
	res = hid_read(handle, buf, 65);

	// Print out the returned buffer.
	short xData = (buf[3] << 8) | buf[2]; //recombined xData into a short
	short yData = (buf[5] << 8) | buf[4]; //recombined xData into a short
	short zData = (buf[7] << 8) | buf[6]; //recombined xData into a short
/* 	printf("x Data is: %d\n", xData);
	printf("y Data is: %d\n", yData);
	printf("z Data is: %d\n", zData); */
	
/* 	for (i = 0; i < 4; i++)
		printf("buf[%d]: %d\n", i, buf[i]); */

		AccelsTable[countReq][0] = xData;
		AccelsTable[countReq][1] = yData;
		AccelsTable[countReq][2] = zData;
}

//Create document of accel data
FILE *ofp;
ofp = fopen("accels.txt", "w");

int ii;
for (ii = 0; ii < numReads; ii++){
	fprintf(ofp, "%d %d %d\r\n", AccelsTable[ii][0], AccelsTable[ii][1], AccelsTable[ii][2]);
}

fclose(ofp);

	// Finalize the hidapi library
	res = hid_exit();

	return 0;
}
