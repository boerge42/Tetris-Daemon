/*
   
   Bibliothek zur Ansteuerung von einem (oder mehreren 
                 kaskatierten) MAX7219
   ====================================================
   Grundgeruest irgendwo im Internet gefunden, der Rest
   von Uwe Berger; 2018
   

  Datenblatt: http://datasheets.maximintegrated.com/en/ds/MAX7219-MAX7221.pdf

   
   ---------
   Have fun!
   
  -------------------------------------------------------------------------------------   
  This file is part of max7219 driver for max7219 driver for WiringPi.
  max7219 driver for WiringPi is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.
  max7219 driver for WiringPi is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  You should have received a copy of the GNU General Public License
  along with max7219 driver for WiringPi.  If not, see <http://www.gnu.org/licenses/>.
  -------------------------------------------------------------------------------------   
*/


#ifndef _MAX7219_H
#define _MAX7219_H

#include <stdint.h>
#include <wiringPiSPI.h>
#include <wiringPi.h>

#define REG_ADDR_NOOP		0x00
#define REG_ADDR_DIGIT0		0x01
#define REG_ADDR_DIGIT1		0x02
#define REG_ADDR_DIGIT2		0x03
#define REG_ADDR_DIGIT3		0x04
#define REG_ADDR_DIGIT4		0x05
#define REG_ADDR_DIGIT5		0x06
#define REG_ADDR_DIGIT6		0x07
#define REG_ADDR_DIGIT7		0x08
#define REG_ADDR_DECODE_MODE	0x09
#define REG_ADDR_INTENSITY	0x0A
#define REG_ADDR_SCAN_LIMIT	0x0B
#define REG_ADDR_SHUTDOWN	0x0C
#define REG_ADDR_TEST		0x0F

#define SHUTDOWN_MODE		0x00
#define NORMAL_MODE		    0x01

#define NO_DECODE		    0x00
#define CODE_B_0		    0x01
#define CODE_B_3_0	    	0x0F
#define CODE_B_7_0		    0xFF

#define DISPLAY_0_ONLY		0x00
#define DISPLAY_0_TO_1		0x01
#define DISPLAY_0_TO_2		0x02
#define DISPLAY_0_TO_3		0x03
#define DISPLAY_0_TO_4		0x04
#define DISPLAY_0_TO_5		0x05
#define DISPLAY_0_TO_6		0x06
#define DISPLAY_0_TO_7		0x07

#define CHAR_BLANK          0xF 
#define CHAR_NEGATIVE       0xA 



// *******************************************************************
// *******************************************************************
// *******************************************************************

int max7219_init(int channel, int speed, int count);

int max7219_send_data(int id, uint8_t address, uint8_t data);

int max7219_send_data_cascade(uint8_t *address, uint8_t *data, int l);
int max7219_send_data_all(uint8_t address, uint8_t data);

int max7219_set_shutdown(int id, uint8_t mode);
int max7219_set_shutdown_all(uint8_t mode);

int max7219_set_decode(int id, uint8_t mode);
int max7219_set_decode_all(uint8_t mode);

int max7219_test(int id);
int max7219_test_all(void);

int max7219_set_intensity(int id, uint8_t intensity);
int max7219_set_intensity_all(uint8_t intensity);

void max7219_clear(uint8_t id);
void max7219_clear_all(void);

void max7219_display_value(uint8_t id, uint8_t first_right_digit, long val, int8_t dp);

#endif
