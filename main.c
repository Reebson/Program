/*
 * GccApplication2.c
 *
 * Created: 18.01.2022 10:36:54
 * Author : piotr
 */ 

#define F_CPU 16000000UL

#include <stdint.h>
#include <avr/io.h>
#include <stdbool.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/sfr_defs.h>
#include <stdlib.h>
#include <string.h>

#include "i2c.h"
#include "LCD_HD44780_IIC.h"
#include "bme280.h"
#include "Menu.h"
#include "USART.h"

#define VREF 5
#define PRESKALER 128 // wartosc preskalera adc
#define DIODE (1<<PD4)

#define Device_Write_address	0xD0	/* Define RTC DS1307 slave write address */
#define Device_Read_address	0xD1	/* Make LSB bit high of slave address for read */
#define C_Ds1307SecondRegAddress_U8   0x00u   // Address to access Ds1307 SEC register
#define C_Ds1307DateRegAddress_U8     0x04u   // Address to access Ds1307 DATE register
#define C_Ds1307ControlRegAddress_U8  0x07u   // Address to access Ds1307 CONTROL register

#define DIODE (1<<PD4)
#define keyLeft (1<<PB0)
#define keyRight (1<<PB1)
#define keyUp (1<<PD5)
#define keyDown (1<<PD6)
#define keyEnter (1<<PD7)

#define Out_1 (1<<PC0)
#define Out_2 (1<<PC1)
#define Out_3 (1<<PC2)
#define Out_4 (1<<PC3)

typedef struct
{
	uint8_t sec;
	uint8_t min;
	uint8_t hour;
	uint8_t weekDay;
	uint8_t date;
	uint8_t month;
	uint8_t year;
}rtct;

//---------------------------------------------------------------------------------------//
//----------------------------FUNCTIONS--------------------------------------------------//
//---------------------------------------------------------------------------------------//

void ADC_init();
uint16_t adc_read(uint8_t ch);
int convertToPercentages(int x);

void RTCinit();
void RTC_SetDateTime(rtct *rtc);
void RTC_GetDateTime(rtct *rtc);
void timerDisplay(rtct *rtc);

void buttonDown();
void buttonUp();
void buttonEnter();
void buttonLeft();
void buttonRight();

int outputEnable_1(int hysteresis);		// sterowanie oswietleniem
void outputEnable_2(int hysteresis);		// sterowanie wentylatorem

uint16_t adc_result0, adc_result1;
volatile int lightFlag = 0;
volatile char globalchar;
volatile int inter = 0;

//---------------------------------------------------------------------------------------//

int main(void)
{
	rtct rtc;
	//char usartChar;
	
	//DDRD |= (1<<PD3); //wyjscie dodatkowe
	DDRD |= (1<<PD4); //wyjscie dodatkowe
	
	DDRC |= Out_1; //wyjscie 230V
	DDRC |= Out_2; //wyjscie 230V
	DDRC |= Out_3; //wyjscie 12V dolne*/
	DDRC |= Out_4; //wyjscie 12V gorne

	PCMSK0 |= (1 << PCINT0) | (1 << PCINT1); // przerwania PCINT0
	PCMSK2 |= (1 << PCINT21) | (1 << PCINT22) | (1 << PCINT23); // przerwania PCINT2
	PCICR |= (1 << PCIE0) | (1 << PCIE2);

	PORTB |= keyLeft;
	PORTB |= keyRight;
	PORTD |= keyUp;
	PORTD |= keyDown;
	PORTD |= keyEnter;

	//SENSORS
	readingParameters.temp = 10; //przypisac do czujnikow
	readingParameters.hum = 20;
	readingParameters.press = 30;
	readingParameters.wg;
	readingParameters.foto;

	thresholdsParameters.temp = 15; //progi
	thresholdsParameters.hum = 25;
	thresholdsParameters.press = 35;
	thresholdsParameters.wg = 45;
	thresholdsParameters.foto = 0;

	ADC_init();
	USART_Init();
	LCDinit();
	RTCinit();
	LCDhome();
	LCDclr();
	
	//RTC
	rtc.hour = 0x16; //  10:40:20 am
	rtc.min =  0x49;
	rtc.sec =  0x0;

	rtc.date = 0x01; //1st Jan 2016
	rtc.month = 0x01;
	rtc.year = 0x16;
	rtc.weekDay = 5; // Friday
	//RTC_SetDateTime(&rtc);  //  10:40:20 am, 1st Jan 2016
	
	sei();

	//displayStartInformation();
	displayMenu(MENU_START,UpDownFlag, 0);	//start menu, indicator=1*/
	
	while (1) {
		
		/*buttonDown();
		buttonUp();
		buttonEnter();
		buttonLeft();
		buttonRight();*/
		
		//if(menuFlag == 0)
			//timerDisplay(&rtc);
			
		PORTD ^= (1<<PD4);
		
		adc_result0 = adc_read(6);		// read adc value at PE2
		adc_result1 = adc_read(7);		// read adc value at PE3
		readingParameters.wg = convertToPercentages(adc_result0);
		readingParameters.foto = convertToPercentages(adc_result1);
		
		outputEnable_2(thresholdsParameters.hysteresis);
		
		if(globalchar == '1') {
			PORTC |= Out_4;
		}
		else if(globalchar == '2')
			PORTC &= ~Out_4;
			
		/*lcd_locate(0,0);
		LCDstring("xd", 2);
		_delay_ms(2000);
		LCDclr();*/
    }
}



void ADC_init() {
	ADCSRA |= 1<<ADEN;  //Enables ADC
	ADCSRA |= 1<<ADPS2 | 1<<ADPS1| 1<<ADPS0;  //Sets ADC Prescalar as 128, i.e. my ADC frequency is 125KHz
	ADCSRA |= 1<<ADSC ;  //Enables ADC interupt and Start the conversion //1<<ADIE
	ADMUX |= (1<<REFS0);
}

uint16_t adc_read(uint8_t ch) {
	// select the corresponding channel 0~7
	// ANDing with ’7? will always keep the value
	// of ‘ch’ between 0 and 7
	ch &= 0b00000111;  // AND operation with 7
	ADMUX = (ADMUX & 0xF8)|ch; // clears the bottom 3 bits before ORing
	
	// start single convertion
	// write ’1? to ADSC
	ADCSRA |= (1<<ADSC);
	
	// wait for conversion to complete
	// ADSC becomes ’0? again
	// till then, run loop continuously
	while(ADCSRA & (1<<ADSC));
	
	return (ADC);
}

/*ISR(ADC_vect) {
	adc_result0 = adc_read(6);      // read adc value at PA0
	adc_result1 = adc_read(7);      // read adc value at PA1
	readingParameters.wg = convertToPercentages(adc_result0);
	readingParameters.foto = convertToPercentages(adc_result1);
}*/

ISR(PCINT0_vect) { //PB0 przerwania przyciskow
	buttonDown();
	buttonRight();
}

ISR(PCINT2_vect) { // przerwania przyciskow
	buttonLeft();
	buttonUp();
	buttonEnter();
}

ISR(USART0_RX_vect) {
	globalchar = UDR0;
	//PORTD |= (1<<PD4);
}

int convertToPercentages(int Value) {
	return Value*0.09766; //przeliczenie wartosci WG na procenty (100/1024) = 0.09766
}

void RTCinit() {
	i2cInit();
	i2cStart();
	i2cSendData(Device_Write_address);
	i2cSendData(C_Ds1307ControlRegAddress_U8);
	i2cSendData(C_Ds1307SecondRegAddress_U8 );
	i2cStop();
}

void RTC_SetDateTime(rtct *rtc) {
	i2cStart();
	i2cSendData(Device_Write_address);
	i2cSendData(C_Ds1307SecondRegAddress_U8 );
	i2cSendData(rtc->sec);
	i2cSendData(rtc->min);
	i2cSendData(rtc->hour);
	i2cSendData(rtc->weekDay);
	i2cSendData(rtc->date);
	i2cSendData(rtc->month);
	i2cSendData(rtc->year);
	
	
	i2cStop();
}

void RTC_GetDateTime(rtct *rtc) {
	i2cStart();
	i2cSendData(Device_Write_address);
	i2cSendData(C_Ds1307SecondRegAddress_U8 );
	i2cStop();
	i2cStart();
	i2cSendData(Device_Read_address);
	
	rtc->sec = i2cReadDataAck();
	rtc->min = i2cReadDataAck();
	rtc->hour = i2cReadDataAck();
	rtc->weekDay = i2cReadDataAck();
	rtc->date = i2cReadDataAck();
	rtc->month = i2cReadDataAck();
	rtc->year = i2cReadDataNotAck();
	
	i2cStop();
}

void timerDisplay(rtct *rtc)
{
	rtct nrtc = *rtc;
	RTC_GetDateTime(&nrtc);
	char s[2];
	
	//hour
	if(nrtc.hour <= 9)
	{
		lcd_locate(3,3);
		LCDstring("0", 1);
		LCDstring(itoa(nrtc.hour, s, 16), 1);
	}
	else
	{
		lcd_locate(3,3);
		LCDstring(itoa(nrtc.hour, s, 16), 2);
	}
	lcd_locate(5,3);
	LCDstring(":", 1);
	
	//min
	if(nrtc.min <= 9)
	{
		lcd_locate(6,3);
		LCDstring("0", 1);
		LCDstring(itoa(nrtc.min, s, 16), 1);
	}
	else
	{
		lcd_locate(6,3);
		LCDstring(itoa(nrtc.min, s, 16), 2);
	}
	
	lcd_locate(8,3);
	LCDstring(":", 1);
	
	//sec
	if(nrtc.sec <= 9)
	{
		lcd_locate(9,3);
		LCDstring("0", 1);
		LCDstring(itoa(nrtc.sec, s, 16), 1);
	}
	else
	{
		lcd_locate(9,3);
		LCDstring(itoa(nrtc.sec, s, 16), 2);
	}
}

void buttonDown() {
	if(bit_is_clear(PINB,0)) //keyDown (1<<PB0)
	{
		//_delay_ms(100);
		UpDownFlag++;
		if(UpDownFlag > 3 && (menuFlag == MENU_START || menuFlag == MENU_OPTIONS))
		UpDownFlag = 1;
		if(UpDownFlag > 6 && (menuFlag == MENU_READING)) //6 wierszy
		UpDownFlag = 1;
		if(UpDownFlag > 7 && (menuFlag == MENU_THRESHOLDS)) //7 wierszy
		UpDownFlag = 1;

		displayMenu(menuFlag,UpDownFlag, 0);
	}
}

void buttonUp() {
	if(bit_is_clear(PIND,6)) // keyUp (1<<PD6)
	{
		//_delay_ms(100);
		UpDownFlag--;
		if(UpDownFlag < 1 && (menuFlag == MENU_START || menuFlag == MENU_OPTIONS))
		UpDownFlag = 3;
		if(UpDownFlag < 1 && (menuFlag == MENU_READING || menuFlag == MENU_THRESHOLDS)) //ustawione narazie dla 4 parametrów
		UpDownFlag = 7;	//ustawione narazie 4

		displayMenu(menuFlag,UpDownFlag, 0);
	}
}

void buttonEnter() {
	if(bit_is_clear(PIND,7)) //keyEnter (1<<PD7)
	{
		//_delay_ms(100);
		if(menuFlag == MENU_START)
		{
			menuFlag = UpDownFlag;
			UpDownFlag = 1;
			displayMenu(menuFlag,UpDownFlag, 0);
		}
		if(menuFlag == MENU_READING)
		{
			if(UpDownFlag == 6)	// back to start menu
			{
				menuFlag = MENU_START;
				UpDownFlag = 1;
				displayMenu(menuFlag,UpDownFlag, 0);
			}
		}
		if(menuFlag == MENU_THRESHOLDS)
		{
			if(UpDownFlag == 7)	// back to start menu
			{
				menuFlag = MENU_START;
				UpDownFlag = 1;
				displayMenu(menuFlag,UpDownFlag, 0);
			}
		}
		if(menuFlag == MENU_OPTIONS)
		{
			if(UpDownFlag == 3)	//back to start menu
			{
				menuFlag = MENU_START;
				UpDownFlag = 1;
				displayMenu(menuFlag,UpDownFlag, 0);
			}
		}
	}
}

void buttonLeft() {
	if(bit_is_clear(PIND,5)) //keyLeft (1<<PD5)
	{
		_delay_ms(100);
		PORTD ^= (1<<PD5);
		if(menuFlag == MENU_THRESHOLDS)
		displayMenu(menuFlag,UpDownFlag,MINUS);
	}
}

void buttonRight() {
	if(bit_is_clear(PINB,1)) //keyRight (1<<PB1)
	{
		_delay_ms(100);
		PORTB ^= (1<<PB1);
		if(menuFlag == MENU_THRESHOLDS)
		displayMenu(menuFlag,UpDownFlag,PLUS);
	}
}

int outputEnable_1(int hysteresis) {		    	// sterowanie oswietleniem PC0
	int newValue = convertToPercentages(adc_result1) + hysteresis;
	
	if(newValue <= thresholdsParameters.foto) // zmienic warunek <= na >=		
		PORTC |= (1<<PC3);		    // domyslnie PC0						//wyjscie zalacza sie	
																						
	else
		PORTC &= ~(1<<PC3);		    // domyslnie PC0							//wyjscie wylaczone
}


void outputEnable_2(int hysteresis) {
	int Lighting = -1;
	int newValue = convertToPercentages(adc_result1);		//przypisanie wartosci foto adc do nowej zmiennej
	
	if(lightFlag == 0 ) {									//flaga = 0, wchodzimy w if
		if(newValue <= thresholdsParameters.foto) {			//jezeli przypisana wartosc jest mniejsza lub rowna ustawionemu progowi
			lightFlag = 1;									//zmien wartosc flagi
			PORTC |= Out_4;									//zapal swiatlo
			_delay_ms(100);
		}
	}
	else { //lightFlag = 1									//jezeli swiatlo juz sie pali
		if(Lighting == -1) {								//flaga = -1, wchodzimy w if
			Lighting = newValue;							//przypisanie wartosci foto adc, po zapaleniu swiatla, do zmiennej
		}
		if(newValue + hysteresis >= Lighting) {				//jezeli aktualna wartosc foto + hysteresa jest wieksza lub rowna wartosci w momencie zapalenia swiatla
			lightFlag = 0;									//zeruj flage
			PORTC &= ~Out_4;								//zgas swiatlo
			Lighting = -1;									//zeruj flage
		}
	}
}
