#include "LCD_HD44780_IIC.h"
#include "i2c.h"

/* -------------------------------------
	HD44780 DEFINITIONS
	----------------------------------- */

/* PCF8574 expander bits 
7	6	5	4	3	2	1	0
RS	RW	E	BT	D7	D6	D5	D4
*/

// definitions of global variables
volatile unsigned char BLval = BACKLIGHT_ON;
volatile unsigned char RSval = RS_COMMAND;
//unsigned char RWval = RW_WRITE;

char 	lcd_buf[LCD_ROWS][LCD_COLS];
char 	lcd_buf_old[LCD_ROWS][LCD_COLS];
uint8_t lcd_buf_x, lcd_buf_y;

/* -------------------------------------
	USER FUNTIONS
	----------------------------------- */
//forms data ready to send
void LCDsendChar(char data)		                        
{
	RSval = RS_DATA;
	sendHalfByteLCD(data >> 4);
	sendHalfByteLCD(data & 0x0F);
	_delay_us(delayCommand);
}
//forms data ready to send
void LCDsendCommand(uint8_t data)	                        
{
	RSval = RS_COMMAND;
	sendHalfByteLCD(data >> 4);
	sendHalfByteLCD(data & 0x0F);
	_delay_us(delayCommand);
}
//Initializes LCD
void LCDinit(void)			                            
{
	unsigned char itr=0;
	
	i2cInit();
	
	RSval = RS_COMMAND;
	for(itr=0;itr<3;itr++){
		sendHalfByteLCD(0x03);
		_delay_ms(6);
	}
	sendHalfByteLCD(0x02);
	_delay_ms(1);
	LCDsendCommand(HD44780_FUNCTION_SET | HD44780_FONT5x10 | HD44780_TWO_LINE | HD44780_4_BIT); // 5x10 font, two line, 4bit interface
	LCDsendCommand(HD44780_DISPLAY_ONOFF | HD44780_DISPLAY_OFF); // turn off display
	LCDsendCommand(HD44780_CLEAR); // clear DDRAM
	_delay_ms(500);
	LCDsendCommand(HD44780_ENTRY_MODE | HD44780_EM_SHIFT_CURSOR | HD44780_EM_INCREMENT);// inkrement addres, move coursore mode
	LCDsendCommand(HD44780_DISPLAY_ONOFF | HD44780_DISPLAY_ON | HD44780_CURSOR_OFF | HD44780_CURSOR_NOBLINK); // turn on LCD, cursor on, blink on
}
//Clears LCD
void LCDclr(void)
{
	LCDsendCommand(HD44780_CLEAR);
}		                        
//LCD cursor home
void LCDhome(void)
{
	LCDsendCommand(HD44780_HOME);
	_delay_ms(10);
}			                            
//Outputs string to LCD
void LCDstring(char* data, uint8_t nOfBytes){
	uint8_t i;
	
	if (!data) return; // check the pointer

	for(i=0; i<nOfBytes; i++)
		LCDsendChar(data[i]);
}
//Cursor to X Y position

void lcd_locate(uint8_t x, uint8_t y) {
	if(y==0)
		LCDsendCommand(HD44780_DDRAM_SET | (x + (0x00)));
	if(y==1)
		LCDsendCommand(HD44780_DDRAM_SET | (x + (0x40)));
	if(y==2)
		LCDsendCommand(HD44780_DDRAM_SET | (x + (0x14)));
	if(y==3)
		LCDsendCommand(HD44780_DDRAM_SET | (x + (0x54)));
}


//shift by n characters Right
void LCDshiftRight(uint8_t n){
	uint8_t i;
	for (i=0;i<n;i++)
		LCDsendCommand(HD44780_DISPLAY_CURSOR_SHIFT | HD44780_SHIFT_DISPLAY | HD44780_SHIFT_RIGHT);
}	
//shift by n characters Left                        
void LCDshiftLeft(uint8_t n){
	uint8_t i;
	for (i=0;i<n;i++)
		LCDsendCommand(HD44780_DISPLAY_CURSOR_SHIFT | HD44780_SHIFT_DISPLAY | HD44780_SHIFT_LEFT);
}	
//Underline cursor ON                            
void LCDcursorOn(void){
	LCDsendCommand(HD44780_DISPLAY_ONOFF | HD44780_DISPLAY_ON | HD44780_CURSOR_ON | HD44780_CURSOR_NOBLINK);
}	
//Underline blinking cursor ON	                            
void LCDcursorOnBlink(void){
	LCDsendCommand(HD44780_DISPLAY_ONOFF | HD44780_DISPLAY_ON | HD44780_CURSOR_ON | HD44780_CURSOR_BLINK);
}
//Cursor OFF
void LCDcursorOFF(void){
	LCDsendCommand(HD44780_DISPLAY_ONOFF | HD44780_DISPLAY_ON | HD44780_CURSOR_OFF | HD44780_CURSOR_NOBLINK);
}	
//LCD blank but not cleared	                        
void LCDblank(void){
	LCDsendCommand(HD44780_DISPLAY_ONOFF | HD44780_DISPLAY_OFF | HD44780_CURSOR_OFF | HD44780_CURSOR_NOBLINK);
}
//LCD visible
void LCDvisible(void){
	LCDsendCommand(HD44780_DISPLAY_ONOFF | HD44780_DISPLAY_ON | HD44780_CURSOR_OFF | HD44780_CURSOR_NOBLINK);
}
//Shift cursor left by n
void LCDcursorLeft(uint8_t n){
	uint8_t i;
	for (i=0;i<n;i++)
		LCDsendCommand(HD44780_DISPLAY_CURSOR_SHIFT | HD44780_SHIFT_CURSOR | HD44780_SHIFT_LEFT);
}
//Shift cursor right by n	                        
void LCDcursorRight(uint8_t n){
	uint8_t i;
	for (i=0;i<n;i++)
		LCDsendCommand(HD44780_DISPLAY_CURSOR_SHIFT | HD44780_SHIFT_CURSOR | HD44780_SHIFT_RIGHT);
}	

void LCDstart_menu() {
	lcd_locate(4,0);
	LCDstring("Sterownik do", 12);
	lcd_locate(4,1);
	LCDstring("monitorowania", 13);
	lcd_locate(2,2);
	LCDstring("warunkow wzrostu", 16);
	lcd_locate(7,3);
	LCDstring("roslin", 6);
	_delay_ms(2000);
	LCDclr();
	lcd_locate(4,0);
	LCDstring("Wykonal:", 8);
	lcd_locate(4,1);
	LCDstring("Piotr Karpik", 12);
	lcd_locate(4,2);
	LCDstring("215498", 6);
	_delay_ms(2000);
	LCDclr();
}

/*void Menu_1 (){
	uint8_t x = 0;
	if(!(PINB & _BV(PB0))) {
		x++;
		if(x==0) {
			LCDclr();
			LCDGotoXY(0,0);
			LCDstring("Wilgotnosc: ", 12);
			LCDGotoXY(0,1);
			LCDstring("Cisnienie: ", 11);
			LCDGotoXY(0,2);
			LCDstring("Wilg. gleby: ", 13);
			LCDGotoXY(0,3);
			LCDstring("Naslonecznienie: ", 17);
		}
	}
		else {
			LCDclr();
			LCDGotoXY(0,0);
			LCDstring("Temperatura: ", 12);
			LCDGotoXY(0,1);
			LCDstring("Wilgotnosc: ", 12);
			LCDGotoXY(0,2);
			LCDstring("Cisnienie: ", 11);
			LCDGotoXY(0,3);
			LCDstring("Wilg. gleby: ", 13);
		}
}*/

void lcd_char(char data)
{
	//LCD_RS_HIGH;
	//lcd_write_byte(data);

	RSval = RS_DATA;
	sendHalfByteLCD(data >> 4);
	sendHalfByteLCD(data & 0x0F);
	_delay_us(delayCommand);

}

void buf_locate(uint8_t x, uint8_t y)
{
	lcd_buf_x = x;
	lcd_buf_y = y;
}

void buf_char(char c)
{
	if (lcd_buf_x < LCD_COLS && lcd_buf_y < LCD_ROWS)
	{
		lcd_buf[lcd_buf_y][lcd_buf_x] = c;
		lcd_buf_x++;
		if (lcd_buf_x == LCD_COLS) {
			lcd_buf_x = 0;
			lcd_buf_y++;
			if (lcd_buf_y == LCD_ROWS)
				lcd_buf_y = 0;
		}
	}
}

void buf_clear(void)
{
	for(uint8_t y=0; y<LCD_ROWS; y++)
	{
		for(uint8_t x=0; x<LCD_COLS; x++)
		{
			lcd_buf[y][x]=' ';
		}
	}
	lcd_buf_x=0; lcd_buf_y=0;
}


void lcd_refresh(void)
{
	static uint8_t locate_flag = 0; // informs when lcd cursor position have to be changed

	for(uint8_t y=0; y<LCD_ROWS; y++)
	{
		lcd_locate( 0, y );

		for(uint8_t x=0; x<LCD_COLS; x++)
		{

			if( lcd_buf[y][x] != lcd_buf_old[y][x] )
			{
				if( !locate_flag )
					lcd_locate( x, y );
				lcd_char( lcd_buf[y][x] );
				lcd_buf_old[y][x] = lcd_buf[y][x];
				locate_flag = 1;
			}
			else
				locate_flag = 0;

		}

	}
}

/* Additional functions */

void buf_str(char *text)
{
	while(*text)
		buf_char(*text++);
}

void buf_str_XY(uint8_t x, uint8_t y, char *text)
{
	buf_locate(x,y);

	while(*text)
		buf_char(*text++);

}


/* -------------------------------------
	BYTE SEND FUNTIONS
	----------------------------------- */
// set output for all 8 I/O of PCF8674 expander
void setOutputs8574(char data){
	i2cStart();
	i2cSendData(PC8574_ADDRESS<<1);
	i2cSendData(data);
	i2cStop();
	_delay_us(10);
}

// send half byte to LCD/HD44780 via PCF8574
void sendHalfByteLCD(char data){
	setOutputs8574(RSval | RW_WRITE | E_ASSERTED | BLval | (data<<4) );
	_delay_us(delayHalfByte);
	setOutputs8574(RSval | RW_WRITE | E_DESSERTED | BLval | (data<<4) );	
	_delay_us(delayHalfByte);
}

