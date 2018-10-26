/* ******************************************************************** 
 * Testprogramm fuer max7219 via WiringPi/SPI
 * ==========================================
 *            Uwe Berger; 2018
 * 
 * 
 * ---------
 * Have fun!
 * 
 *
 * ********************************************************************
 */

#include "max7219.h"

#define DISPLAY_COUNT 3

#define SPI_CHANNEL 0

int main(void)
{

	// WiringPi init (allein fuer SPI nicht noetig, aber wer weiss, was noch kommt ;-)
	wiringPiSetup();
	
	// SPI init
	max7219_init(SPI_CHANNEL, 500000, DISPLAY_COUNT);

	// Helligkeit, Zeichencode, Anschalten, Scanbreite
	max7219_set_intensity_all(4);
	max7219_set_decode_all(CODE_B_7_0);
	max7219_set_shutdown_all(NORMAL_MODE);
	max7219_send_data_all(REG_ADDR_SCAN_LIMIT, DISPLAY_0_TO_7);
	max7219_clear_all();
	
	/*
	max7219_send_data(0, REG_ADDR_DIGIT0, 0);
	max7219_send_data(1, REG_ADDR_DIGIT0, 1);
	max7219_send_data(2, REG_ADDR_DIGIT0, 2);
	*/
	
	max7219_display_value(0, 0, 42, 0);
	max7219_display_value(0, 4, 1504, 1);

	max7219_display_value(1, 0, 42, 1);
	max7219_display_value(1, 4, 1504, 2);

	max7219_display_value(2, 0, 1504, 3);
	max7219_display_value(2, 4, 42, -1);
	

	return 0;
}
