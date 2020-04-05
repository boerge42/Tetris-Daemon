/* ********************************************************************
 *
 * Uhren fuer 64x32-LED-Matrix
 * ===========================
 *      Uwe Berger; 2020
 *
 * --> Controller: ESP8266 (WEMOS D1 mini)
 *
 * --> 64x32-LED-Matrix: https://github.com/2dom/PxMatrix
 * 
 * 
 * ---------
 * Have fun!
 * 
 *
 * ********************************************************************
 */

// Clock
#define CLOCK_DIGIT_DX  15
#define CLOCK_DIGIT_DY  23

uint16_t clock_textColor = display.color565(100, 100, 100);
uint16_t clock_backgroundColor = display.color565(0, 0, 30);
uint16_t clock_BLACK = display.color565(0, 0, 0);
uint16_t clock_YELLOW = display.color565(255, 255, 0);
uint16_t clock_RED = display.color565(255, 0, 0);
uint16_t clock_GREEN = display.color565(0, 255, 0);
uint16_t clock_BLUE = display.color565(0, 0, 255);
uint16_t clock_WHITE = display.color565(100, 100, 100);

uint8_t clock_type = 1;


// **************************************************************
void draw_clock_berlin(uint8_t h, uint8_t m, uint8_t s)
{
  uint8_t i;
  uint8_t x = 1;
  
  // Bildschirm loeschen
  display.fillScreen(clock_BLACK);
  // Sekunde
  if ((s % 2) != 1) {
	 display.fillRoundRect(x, 12, 5, 8, 1, clock_YELLOW);
  }
  // Stunden 
  // Sonderregelung
  if (h == 0) h = 24;
  x = 9;
  for (i=0; i<(h/5); i++) {
	display.fillRect(x, 31-((i+1)*9)+5, 5, 5, clock_RED); 
  }
  x = 17;
  for (i=0; i<(h%5); i++) {
	display.fillRect(x, 31-((i+1)*9)+5, 5, 5, clock_RED); 
  }
  // Minuten
  x = 25;
  for (i=0; i<(m/5); i++) {
	if ((i+1)%3) {
	  display.fillRect(x, 31-((i+1)*3)+2, 5, 2, clock_YELLOW); 
	} else {
	  display.fillRect(x, 31-((i+1)*3)+2, 5, 2, clock_RED); 
	}
  }
  x = 33;
  for (i=0; i<(m%5); i++) {
	display.fillRect(x, 31-((i+1)*9)+5, 5, 5, clock_YELLOW); 
  }
  display.showBuffer();
}


// **************************************************************
void draw_clock_triangular(uint8_t h, uint8_t m)
{
  
  int mins;
  uint8_t i;
  uint16_t color;
  
  // Vormittag/Nachmittag
  if (h < 12) {
	color = clock_BLUE;
  } else {
	// Farbe
	h = h-12;
	color = clock_RED;
  }
  mins = h*60+m;
  display.fillScreen(clock_BLACK);
  if (mins >= 360) {
    display.fillCircle(3, 16, 2, color);
    mins= mins-360;	  
  } else { 
    display.drawCircle(3, 16, 2, clock_textColor);
  }
  
  for (i=0; i<2; i++) {
    if (mins >= 120) {
      display.fillCircle(9, 19-(i*6), 2, color);
      mins= mins-120;	  
    } else { 
      display.drawCircle(9, 19-(i*6), 2, clock_textColor);
    }
  } 

  for (i=0; i<3; i++) {
    if (mins >= 30) {
      display.fillCircle(15, 22-(i*6), 2, color);
      mins= mins-30;	  
    } else { 
      display.drawCircle(15, 22-(i*6), 2, clock_textColor);
    }
  } 

  for (i=0; i<4; i++) {
    if (mins >= 6) {
      display.fillCircle(21, 25-(i*6), 2, color);
      mins= mins-6;	  
    } else { 
      display.drawCircle(21, 25-(i*6), 2, clock_textColor);
    }
  } 
  
  for (i=0; i<5; i++) {
    if (mins >= 1) {
      display.fillCircle(27, 28-(i*6), 2, color);
      mins= mins-1;	  
    } else { 
      display.drawCircle(27, 28-(i*6), 2, clock_textColor);
    }
  } 
  display.showBuffer();	
}

// **************************************************************
void draw_clock_fibonacci(uint8_t h, uint8_t m)
{
  uint16_t c[5]   = {clock_WHITE, clock_WHITE, clock_WHITE, clock_WHITE, clock_WHITE};
  uint8_t single_mins = m%5;
  uint8_t i;

  if (h>12) h=h-12;
  m = m/5;
  display.fillScreen(clock_BLACK);
  // Hintergrundfarbe
  display.fillRect(0, 0, 20, 32, clock_WHITE);
  // Stunden
  if (h>=5) {
	c[4] = clock_RED;
	h=h-5;
  }  
  if (h>=3) {
	c[3] = clock_RED;
	h=h-3;
  }  
  if (h>=2) {
	c[2] = clock_RED;
	h=h-2;
  }  
  if (h>=1) {
	c[1] = clock_RED;
	h=h-1;
  }  
  if (h>=1) {
	c[0] = clock_RED;
	h=h-1;
  }  
  // Minuten
  if (m>=5) {
	m=m-5;
	if (c[4] == clock_WHITE) {
	  c[4] = clock_GREEN;	
	} else {
	  c[4] = clock_BLUE;	
	}
  }  
  if (m>=3) {
	m=m-3;
	if (c[3] == clock_WHITE) {
	  c[3] = clock_GREEN;	
	} else {
	  c[3] = clock_BLUE;	
	}
  }  
  if (m>=2) {
	m=m-2;
	if (c[2] == clock_WHITE) {
	  c[2] = clock_GREEN;	
	} else {
	  c[2] = clock_BLUE;	
	}
  }  
  if (m>=1) {
	m=m-1;
	if (c[1] == clock_WHITE) {
	  c[1] = clock_GREEN;	
	} else {
	  c[1] = clock_BLUE;	
	}
  }  
  if (m>=1) {
	m=m-1;
	if (c[0] == clock_WHITE) {
	  c[0] = clock_GREEN;	
	} else {
	  c[0] = clock_BLUE;	
	}
  }  
  // Fibonacci-Uhr zeichnen
  display.fillRect(0, 0, 20, 20, c[4]);
  display.fillRect(8, 20, 12, 12, c[3]);
  display.fillRect(0, 24, 8, 8, c[2]);
  display.fillRect(4, 20, 4, 4, c[1]);
  display.fillRect(0, 20, 4, 4, c[0]);
  // Einzelminuten
  for (i=0; i<single_mins; i++) {
    display.fillRect(22, 28-i*6, 4, 4, clock_GREEN);
  }
  display.showBuffer();	
}


// **************************************************************
void draw_clock_stencil_digit(uint8_t xp, uint8_t yp, uint8_t v)
{
  uint8_t x = CLOCK_DIGIT_DY * xp + xp;
  uint8_t y = CLOCK_DIGIT_DX * yp + yp;
	
  switch (v) {
	case 0:
	  display.fillRect(x+CLOCK_DIGIT_DX/2-1, y+CLOCK_DIGIT_DY/3-1, CLOCK_DIGIT_DY/3+3, 2, clock_textColor);
	  break;  
	case 1:
	  display.fillRect(x, y+CLOCK_DIGIT_DY/3-1,  CLOCK_DIGIT_DY-1, 2, clock_textColor);
	  break;  
	case 2:
	  display.fillRect(x+(CLOCK_DIGIT_DY/3)-1, y+CLOCK_DIGIT_DX/2-1, 2, CLOCK_DIGIT_DX/2+1, clock_textColor);
	  display.fillRect(x+2*(CLOCK_DIGIT_DY/3), y, 2, CLOCK_DIGIT_DX/2+1, clock_textColor);
	  break;  
	case 3:
	  display.fillRect(x+(CLOCK_DIGIT_DY/3)-1, y+CLOCK_DIGIT_DX/2-1, 2, CLOCK_DIGIT_DX/2+1, clock_textColor);
	  display.fillRect(x+2*(CLOCK_DIGIT_DY/3),   y+CLOCK_DIGIT_DX/2-1, 2, CLOCK_DIGIT_DX/2+1, clock_textColor);
	  break;  
	case 4:
	  display.fillRect(x, y+CLOCK_DIGIT_DY/3-1, CLOCK_DIGIT_DY/3+1, 2, clock_textColor);
	  display.fillRect(x+2*(CLOCK_DIGIT_DY/3), y+CLOCK_DIGIT_DX/2-1, 2, CLOCK_DIGIT_DX/2+1, clock_textColor);
	  display.fillRect(x+2*(CLOCK_DIGIT_DY/3), y+CLOCK_DIGIT_DY/3-1, CLOCK_DIGIT_DY/3+1, 2, clock_textColor);
	  break;  
	case 5:
	  display.fillRect(x+(CLOCK_DIGIT_DY/3)-1, y, 2, CLOCK_DIGIT_DX/2+1, clock_textColor);
	  display.fillRect(x+2*(CLOCK_DIGIT_DY/3),   y+CLOCK_DIGIT_DX/2-1, 2, CLOCK_DIGIT_DX/2+1, clock_textColor);
	  break;  
	case 6:
	  display.fillRect(x+(CLOCK_DIGIT_DY/3)-1, y, 2, CLOCK_DIGIT_DX/2+1, clock_textColor);
	  display.fillRect(x+2*(CLOCK_DIGIT_DY/3), y+CLOCK_DIGIT_DX/2-1, 2, 2, clock_textColor);
	  break;  
	case 7:
	  display.fillRect(x+(CLOCK_DIGIT_DY/3)-1, y+CLOCK_DIGIT_DX/2-1, 2, CLOCK_DIGIT_DX/2+1, clock_textColor);
	  display.fillRect(x+CLOCK_DIGIT_DX/2-1, y+CLOCK_DIGIT_DY/3-1, 2*(CLOCK_DIGIT_DY/3)+2, 2, clock_textColor);
      break;  
	case 8:
	  display.fillRect(x+(CLOCK_DIGIT_DY/3)-1, y+CLOCK_DIGIT_DX/2-1, 2, 2, clock_textColor);
	  display.fillRect(x+2*(CLOCK_DIGIT_DY/3),   y+CLOCK_DIGIT_DX/2-1, 2, 2, clock_textColor);
	  break;  
	case 9:
	  display.fillRect(x+(CLOCK_DIGIT_DY/3)-1, y+CLOCK_DIGIT_DX/2-1, 2, 2, clock_textColor);
	  display.fillRect(x+2*(CLOCK_DIGIT_DY/3), y+CLOCK_DIGIT_DX/2-1, 2, CLOCK_DIGIT_DX/2+1, clock_textColor);
	  break;  
  }
}

// **************************************************************
void draw_clock_stencil(uint8_t h, uint8_t m)
{
  // Bildschirm loeschen
  display.fillScreen(clock_BLACK);
  // Hintergrund  
  display.fillRect(0, 0, 2*CLOCK_DIGIT_DY, 2*CLOCK_DIGIT_DX, clock_backgroundColor);
  // Kreuz  
  display.fillRect(0, CLOCK_DIGIT_DX-1, 2*CLOCK_DIGIT_DY, 2, clock_textColor);
  display.fillRect(CLOCK_DIGIT_DY-1, 0, 2, 2*CLOCK_DIGIT_DX, clock_textColor);
  // Stunde/Minute
  draw_clock_stencil_digit(0, 0, (h%10));  // --> Stunden-Einer
  draw_clock_stencil_digit(0, 1, (h/10));  // --> Stunden-Zehner
  draw_clock_stencil_digit(1, 0, (m%10));  // --> Minuten-Einer
  draw_clock_stencil_digit(1, 1, (m/10));  // --> Minuten-Zehner
  // anzeigen  
  display.showBuffer(); 
}


// **************************************************************
void draw_clock_text()
{
  display.fillScreen(clock_backgroundColor);
  display.setTextColor(clock_textColor);
  display.setCursor(1, 1);
  display.print(myTZ.dateTime("l"));
  display.setCursor(1, 11);
  display.print(myTZ.dateTime("d.m.Y"));
  display.setCursor(1, 21);
  display.print(myTZ.dateTime("H:i:s"));
  display.showBuffer();
}

// **************************************************************
void draw_clock()
{
  switch (clock_type) {
	case 0:
	  draw_clock_text();
	  break;
	case 1:
      draw_clock_stencil(myTZ.hour(), myTZ.minute());
	  break; 
	case 2:
      draw_clock_berlin(myTZ.hour(), myTZ.minute(), myTZ.second());
	  break; 
	case 3:
      draw_clock_triangular(myTZ.hour(), myTZ.minute());
	  break; 
	case 4:
      draw_clock_fibonacci(myTZ.hour(), myTZ.minute());
	  break; 
	default:
	  draw_clock_text();
	  break; 
  }
}
