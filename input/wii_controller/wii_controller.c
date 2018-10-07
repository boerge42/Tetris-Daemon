/*
  Tetris-Input mit WII-Controller 
  ===============================
          Uwe Berger; 2018

  
  ...soll mal vielleicht werden...;-)
  
  
  ---------
  Have fun! 
 
 *----------------------------------------------------------------------
 * Copyright (c) 2018 Uwe Berger - bergeruw(at)gmx.net
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 *----------------------------------------------------------------------
*/

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>


#include <wiringPi.h>
#include <wiringPiI2C.h>

// ***************************************************
void byte2binstr(char s[9], uint8_t b)
{
	memset(s, 0, 9);
	if (b & 1)   s[7] = '1'; else s[7] = '0';
	if (b & 2)   s[6] = '1'; else s[6] = '0';
	if (b & 4)   s[5] = '1'; else s[5] = '0';
	if (b & 8)   s[4] = '1'; else s[4] = '0';
	if (b & 16)  s[3] = '1'; else s[3] = '0';
	if (b & 32)  s[2] = '1'; else s[2] = '0';
	if (b & 64)  s[1] = '1'; else s[1] = '0';
	if (b & 128) s[0] = '1'; else s[0] = '0';
}

int main(int argc, char **argv)
{
	
	int fd; 
	uint8_t i;
	uint8_t buf[6];
	char bin_str[9];
	
	wiringPiSetup();
	fd = wiringPiI2CSetup(0x52);
	
    if (fd < 0) {
        printf("Error setting up I2C: %d\n", errno);
        exit(0);
    }	
	
/*	
	// WII-Controller Init-Sequenz
	wiringPiI2CWriteReg8(fd, 0xF0, 0x55);	
	delay(1);
	wiringPiI2CWriteReg8(fd, 0xFB, 0x00);	
	delay(1);
*/


	wiringPiI2CWriteReg8(fd, 0x40, 0x00);	
	delayMicroseconds(20);


	// Controller-Typ auslesen
//	wiringPiI2CWrite(fd, 0xFA);
//	delay(1);
//	for (i=0; i<6; i++) {
//		bytes[i]=wiringPiI2CRead(fd);
//	}

//	for (i=0; i<6; i++) {
//		printf("%i > %i\n", i, bytes[i]);
//	}


	while (1) {   
        if (wiringPiI2CWrite(fd, 0x00)<0) puts("!!!");
        delayMicroseconds(100);
        for (i=0; i<6; i++) {
            buf[i] = wiringPiI2CRead(fd);
        }
        for (i=0; i<6; i++) {
			byte2binstr(bin_str, buf[i]);
			printf("%s ", bin_str);
        }
        puts("");
        delay(250);
	}   

	return 0;
}
