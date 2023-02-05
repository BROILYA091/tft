/*
  ST7781R TFT Library. 

  2011 Copyright (c) Seeed Technology Inc.
 
  Authors: Albert.Miao, Visweswara R (with initializtion code from TFT vendor)
  
  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/
/*
  Modified record:
  2012.3.27 by Frankie.Chu
    Add funtion:setDisplayDirect.
    Add more conditional statements in funtions,fillRectangle,drawChar,drawString 
    to deal with different directions displaying.
*/
#include <wiringPi.h>
#include <stdlib.h>
#include "tft.h" 
#include "font.h"

void Wrt_byte(unsigned char value) 
{
unsigned char i,rslt,bty;
		
		bty= value;

		rslt = bty & 0x01;
		if(rslt){digitalWrite(0, HIGH);}
		else{digitalWrite(0, LOW) ;}

		for (i = 1 ; i< 8 ; ++i)
		{
		  bty = (bty>>1) ;
			
		  rslt = bty & 0x01;
		  
		  if(rslt){
			  digitalWrite(i, HIGH);}
		  else{ 
			  digitalWrite(i, LOW) ;}
		  
		}
		
	/*	bty= value;

		for (i = 0 ; i< 8 ; ++i)
		{
		  if (i>0){bty = (bty>>1) ;}
			
		  rslt = bty & 0x01;
		  
		  if(rslt){
			  digitalWrite(i, HIGH);}
		  else{ 
			  digitalWrite(i, LOW) ;}
		  
		
		}*/
		return ;
}

void ResetTFT(void) //pulse reset signal to LCD
{
	digitalWrite(RST,HIGH);
	digitalWrite(RST,LOW);
	delay(50);
	digitalWrite(RST,HIGH);
}


void sendCommand(unsigned int index)
{
	digitalWrite(RS,LOW);
	digitalWrite(CS,LOW);	 
	
	Wrt_byte(index>>8);
	
	digitalWrite(WR,LOW);
	digitalWrite(WR,HIGH);	
	
	Wrt_byte(index);
	
	digitalWrite(WR,LOW);
	digitalWrite(WR,HIGH);
	digitalWrite(CS,HIGH);

}

void sendData(unsigned int data)
{
  
        digitalWrite(RS,HIGH);
	digitalWrite(CS,LOW);	 
	
	Wrt_byte(data>>8);
	
	digitalWrite(WR,LOW);
	digitalWrite(WR,HIGH);	
		
	Wrt_byte(data);
	
	digitalWrite(WR,LOW);
	digitalWrite(WR,HIGH);
	digitalWrite(CS,HIGH);
}

void LCD_CtrlWrite_ILI9325(unsigned int cmd, unsigned int data)
{
	sendCommand(cmd);
	sendData(data);
}


void ILI9325_CPT28_Initial(void)
{
	wiringPiSetup();
    
	pinMode (0,OUTPUT); //GPIO0 (pin11)
	pinMode (1,OUTPUT); //GPIO1 (pin12)
	pinMode (2,OUTPUT); //GPIO2 (pin13)
	pinMode (3,OUTPUT); //GPIO3 (pin15)
	pinMode (4,OUTPUT); //GPIO4 (pin16)
	pinMode (5,OUTPUT); //GPIO5 (pin18)
	pinMode (6,OUTPUT); //GPIO6 (pin22)
	pinMode (7,OUTPUT); //GPIO7 (pin07)
	
	pinMode(RS,OUTPUT);
	pinMode(WR,OUTPUT);
	pinMode(CS,OUTPUT);
	pinMode(RST,OUTPUT);
	
	// VCI=2.8V
//************* Reset LCD Driver ****************//
	digitalWrite(RST,HIGH);//LCD_nRESET = 1;
	delay(1); // Delay 1ms
	digitalWrite(RST,LOW);//LCD_nRESET = 0;
	delay(10); // Delay 10ms   // This delay time is necessary
	digitalWrite(RST,HIGH);//LCD_nRESET = 1;
	delay(50); // Delay 50 ms
	
//************* Start Initial Sequence **********//
	LCD_CtrlWrite_ILI9325(0x00E3, 0x3008); // Set internal timing
	LCD_CtrlWrite_ILI9325(0x00E7, 0x0012); // Set internal timing
	LCD_CtrlWrite_ILI9325(0x00EF, 0x1231); // Set internal timing
	LCD_CtrlWrite_ILI9325(0x0001, 0x0100); // set SS and SM bit
	LCD_CtrlWrite_ILI9325(0x0002, 0x0700); // set 1 line inversion
	LCD_CtrlWrite_ILI9325(0x0003, 0x1030); // set GRAM write direction and BGR=1.
	LCD_CtrlWrite_ILI9325(0x0004, 0x0000); // Resize register
	LCD_CtrlWrite_ILI9325(0x0008, 0x0207); // set the back porch and front porch
	LCD_CtrlWrite_ILI9325(0x0009, 0x0000); // set non-display area refresh cycle ISC[3:0]
	LCD_CtrlWrite_ILI9325(0x000A, 0x0000); // FMARK function
	LCD_CtrlWrite_ILI9325(0x000C, 0x0000); // RGB interface setting
	LCD_CtrlWrite_ILI9325(0x000D, 0x0000); // Frame marker Position
	LCD_CtrlWrite_ILI9325(0x000F, 0x0000); // RGB interface polarity

//*************Power On sequence ****************//
	LCD_CtrlWrite_ILI9325(0x0010, 0x0000); // SAP, BT[3:0], AP, DSTB, SLP, STB
	LCD_CtrlWrite_ILI9325(0x0011, 0x0007); // DC1[2:0], DC0[2:0], VC[2:0]
	LCD_CtrlWrite_ILI9325(0x0012, 0x0000); // VREG1OUT voltage
	LCD_CtrlWrite_ILI9325(0x0013, 0x0000); // VDV[4:0] for VCOM amplitude
	delay(200); // Dis-charge capacitor power voltage
	LCD_CtrlWrite_ILI9325(0x0010, 0x1290); // SAP, BT[3:0], AP, DSTB, SLP, STB
	LCD_CtrlWrite_ILI9325(0x0011, 0x0227); // DC1[2:0], DC0[2:0], VC[2:0]
	delay(50); // Delay 50ms
	LCD_CtrlWrite_ILI9325(0x0012, 0x001A); // Internal reference voltage= Vci;
	delay(50); // Delay 50ms
	LCD_CtrlWrite_ILI9325(0x0013, 0x1800); // Set VDV[4:0] for VCOM amplitude
	LCD_CtrlWrite_ILI9325(0x0029, 0x0028); // Set VCM[5:0] for VCOMH
	LCD_CtrlWrite_ILI9325(0x002B, 0x000C); // Set Frame Rate
	delay(50); // Delay 50ms
	LCD_CtrlWrite_ILI9325(0x0020, 0x0000); // GRAM horizontal Address
	LCD_CtrlWrite_ILI9325(0x0021, 0x0000); // GRAM Vertical Address

// ----------- Adjust the Gamma Curve ----------//
	LCD_CtrlWrite_ILI9325(0x0030, 0x0000);
	LCD_CtrlWrite_ILI9325(0x0031, 0x0305);
	LCD_CtrlWrite_ILI9325(0x0032, 0x0003);
	LCD_CtrlWrite_ILI9325(0x0035, 0x0304);
	LCD_CtrlWrite_ILI9325(0x0036, 0x000F);
	LCD_CtrlWrite_ILI9325(0x0037, 0x0407);
	LCD_CtrlWrite_ILI9325(0x0038, 0x0204);
	LCD_CtrlWrite_ILI9325(0x0039, 0x0707);
	LCD_CtrlWrite_ILI9325(0x003C, 0x0403);
	LCD_CtrlWrite_ILI9325(0x003D, 0x1604);
//------------------ Set GRAM area ---------------//
	LCD_CtrlWrite_ILI9325(0x0050, 0x0000); // Horizontal GRAM Start Address
	LCD_CtrlWrite_ILI9325(0x0051, 0x00EF); // Horizontal GRAM End Address
	LCD_CtrlWrite_ILI9325(0x0052, 0x0000); // Vertical GRAM Start Address
	LCD_CtrlWrite_ILI9325(0x0053, 0x013F); // Vertical GRAM Start Address
	LCD_CtrlWrite_ILI9325(0x0060, 0xA700); // Gate Scan Line
	LCD_CtrlWrite_ILI9325(0x0061, 0x0001); // NDL,VLE, REV
	LCD_CtrlWrite_ILI9325(0x006A, 0x0000); // set scrolling line
//-------------- Partial Display Control ---------//
	LCD_CtrlWrite_ILI9325(0x0080, 0x0000);
	LCD_CtrlWrite_ILI9325(0x0081, 0x0000);
	LCD_CtrlWrite_ILI9325(0x0082, 0x0000);
	LCD_CtrlWrite_ILI9325(0x0083, 0x0000);
	LCD_CtrlWrite_ILI9325(0x0084, 0x0000);
	LCD_CtrlWrite_ILI9325(0x0085, 0x0000);
//-------------- Panel Control -------------------//
	LCD_CtrlWrite_ILI9325(0x0090, 0x0010);
	LCD_CtrlWrite_ILI9325(0x0092, 0x0600);
	LCD_CtrlWrite_ILI9325(0x0007, 0x0133); // 262K color and display ON
}


void paintScreenBlack(void)
{
	unsigned char i;
	unsigned int f;
    for(i=0;i<2;i++)
    {
        for(f=0;f<38400;f++)
        {
            sendData(BLACK);
        }
    }
}

void exitStandBy(void)
{
    sendCommand(0x0010);
    sendData(0x14E0);
    delay(100);
    sendCommand(0x0007);
    sendData(0x0133);
}

void setOrientation(unsigned int HV)//horizontal or vertical
{
    sendCommand(0x03);
    if(HV==1)//vertical
    {
        sendData(0x5038);
    }
    else//horizontal
    {
        sendData(0x5030);
    }
    sendCommand(0x0022); //Start to write to display RAM
}

void setDisplayDirect(unsigned char Direction) 
{
  DisplayDirect = Direction;
}

void setXY(unsigned int poX, unsigned int poY)
{
    sendCommand(0x0020);//X
    sendData(poX);
    sendCommand(0x0021);//Y
    sendData(poY);
    sendCommand(0x0022);//Start to write to display RAM
}

void setPixel(unsigned int poX, unsigned int poY,unsigned int color)
{
    setXY(poX,poY);
    sendData(color);
}

void drawCircle(int poX, int poY, int r,unsigned int color)
{
    int x = -r, y = 0, err = 2-2*r, e2; 
    do {
        setPixel(poX-x, poY+y,color); 
        setPixel(poX+x, poY+y,color); 
        setPixel(poX+x, poY-y,color); 
        setPixel(poX-x, poY-y,color); 
        e2 = err;
        if (e2 <= y) { 
            err += ++y*2+1;
            if (-x == y && e2 <= x) e2 = 0; 
        }
        if (e2 > x) err += ++x*2+1; 
    } while (x <= 0);
}

void fillCircle(int poX, int poY, int r,unsigned int color)
{
    int x = -r, y = 0, err = 2-2*r, e2;
    do {

        drawVerticalLine(poX-x,poY-y,2*y,color);
        drawVerticalLine(poX+x,poY-y,2*y,color);

        e2 = err;
        if (e2 <= y) {
            err += ++y*2+1;
            if (-x == y && e2 <= x) e2 = 0;
        }
        if (e2 > x) err += ++x*2+1;
    } while (x <= 0);

}


void drawLine(unsigned int x0,unsigned int y0,unsigned int x1,unsigned int y1,unsigned int color)
{
    int x = x1-x0;
    int y = y1-y0;
    int dx = abs(x), sx = x0<x1 ? 1 : -1;
    int dy = -abs(y), sy = y0<y1 ? 1 : -1;
    int err = dx+dy, e2; /* error value e_xy */
    for (;;){ /* loop */
        setPixel(x0,y0,color);
        e2 = 2*err;
        if (e2 >= dy) { /* e_xy+e_x > 0 */
            if (x0 == x1) break;
            err += dy; x0 += sx;
        }
        if (e2 <= dx) { /* e_xy+e_y < 0 */
            if (y0 == y1) break;
            err += dx; y0 += sy;
        }
    }
}


void drawVerticalLine(unsigned int poX, unsigned int poY,unsigned int length,unsigned int color)
{
	unsigned int i;
    setXY(poX,poY);
    setOrientation(1);
    if(length+poY>MAX_Y)
    {
        length=MAX_Y-poY;
    }

    for(i=0;i<length;i++)
    {
        sendData(color);
    }
}

void  drawHorizontalLine(unsigned int poX, unsigned int poY,unsigned int length,unsigned int color)
{
	unsigned int i;
    setXY(poX,poY);
    setOrientation(0);
    if(length+poX>MAX_X)
    {
        length=MAX_X-poX;
    }
    for(i=0;i<length;i++)
    {
        sendData(color);
    }
}


void drawRectangle(unsigned int poX, unsigned int poY, unsigned int length,unsigned int width,unsigned int color)
{
    drawHorizontalLine(poX, poY, length, color);
    drawHorizontalLine(poX, poY+width, length, color);

    drawVerticalLine(poX, poY, width,color);
    drawVerticalLine(poX + length, poY, width,color);
}

void fillRectangle(unsigned int poX, unsigned int poY, unsigned int length, unsigned int width, unsigned int color)
{
	unsigned int i;
    for(i=0;i<width;i++)
    {
        if(DisplayDirect == LEFT2RIGHT)
          drawHorizontalLine(poX, poY+i, length, color);
          else if (DisplayDirect ==  DOWN2UP)
            drawHorizontalLine(poX, poY-i, length, color);
            else if(DisplayDirect == RIGHT2LEFT)
              drawHorizontalLine(poX, poY-i, length, color);
              else if(DisplayDirect == UP2DOWN)
                drawHorizontalLine(poX, poY+i, length, color);
              
    }
}

void drawChar(unsigned char ascii,unsigned int poX, unsigned int poY,unsigned int size, unsigned int fgcolor)
{
    unsigned char i,temp, f;
    setXY(poX,poY);
    
    if((ascii < 0x20)||(ascii > 0x7e))//Unsupported char.
    {
        ascii = '?';
    }
    for(i=0;i<8;i++)
    {
        //temp = pgm_read_byte(&simpleFont[ascii-0x20][i]);
        temp = font[ascii-0x20][i];
        
        for(f=0;f<8;f++)
        {
            if((temp>>f)&0x01)
            {
                if(DisplayDirect == LEFT2RIGHT)
                  fillRectangle(poX+i*size, poY+f*size, size, size, fgcolor);
                  else if(DisplayDirect == DOWN2UP)
                    fillRectangle(poX+f*size, poY-i*size, size, size, fgcolor);
                    else if(DisplayDirect == RIGHT2LEFT)
                      fillRectangle(poX-i*size, poY-f*size, size, size, fgcolor);
                      else if(DisplayDirect == UP2DOWN)
                        fillRectangle(poX-f*size, poY+i*size, size, size, fgcolor);
            }

        }
    }
}

void drawString(char *string,unsigned int poX, unsigned int poY,unsigned int size,unsigned int fgcolor)
{
	unsigned char i;
    while(*string)
    {
        for(i=0;i<8;i++)
        {
            drawChar(*string, poX, poY, size, fgcolor);
        }
        string++;//*string++;
        if(DisplayDirect == LEFT2RIGHT)
        {
          if(poX < MAX_X)
          {
              poX+=8*size; // Move cursor right
          }
        }
          else if(DisplayDirect == DOWN2UP)
          {
            if(poY > 0)
            {
                poY-=8*size; // Move cursor right
            }
          }
          else if(DisplayDirect == RIGHT2LEFT)
        {
          if(poX > 0)
          {
              poX-=8*size; // Move cursor right
          }
        }
          else if(DisplayDirect == UP2DOWN)
          {
            if(poY < MAX_Y)
            {
                poY+=8*size; // Move cursor right
            }
          }
          
          

    }
}


