/*
 * Menu.h
 *
 * Created: 21.05.2022 19:13:51
 *  Author: piotr
 */ 

#define MENU_START		0
#define MENU_READING	1
#define MENU_THRESHOLDS	2
#define MENU_OPTIONS	3
#define TIME			4
#define ERROR			-1

#define MINUS			-1
#define PLUS			 1

void displayStartInformation(void);
void displayReadingMenu(int indicator);
void displayMenu(int menuflag, int indicator, int change);
void displayThresholdsMenu(int indicator, int change);

volatile int menuFlag = MENU_START;
volatile int UpDownFlag = 1;
volatile int leftRightFlag = 0;

typedef struct Parameters{
	int temp;
	int hum;
	int press;
	int wg;
	int foto;
	int hysteresis;
} parameters;

volatile parameters readingParameters;
volatile parameters thresholdsParameters;

void displayMenu(int flag, int indicator, int change) {

	int indicator_2;

	indicator--;
	indicator_2 = indicator;

	if(indicator > 3) {
		indicator = 3;
	}

	LCDclr();
	lcd_locate(0,indicator);
	LCDstring("-> ", 3); //3

	switch(flag) {
		case MENU_START:	//menuflag = 0
		{
			lcd_locate(3,0);
			LCDstring("Odczyt pomiarow", 15);	//3 - 17 bit LCD
			lcd_locate(3,1);
			LCDstring("Progi", 5);				//3 - 7 bit LCD
			lcd_locate(3,2);
			LCDstring("Ustawienia", 10);		//3 - 12 bit LCD
		}
		break;
		case MENU_READING:	//menuflag = 1
		{
			displayReadingMenu(indicator_2);
		}
		break;
		case MENU_THRESHOLDS: //menuflag = 2
		{
			displayThresholdsMenu(indicator_2, change);
		}
		break;
		case MENU_OPTIONS: //menuflag = 3
		{
			lcd_locate(3,0);
			LCDstring("Buzzer: ", 8);		//3 - 7 bit LCD
			lcd_locate(3,1);
			LCDstring("BT: ", 4);		//3 - 7 bit LCD
			lcd_locate(3,2);
			LCDstring("Cofnij", 6);		//3 - 8 bit LCD
		}
		break;
		default:	//menuflag != 0,1,2,3
		{
			//ERROR -> in 2 second back to start menu
			LCDclr();
			lcd_locate(0,0);
			LCDstring("ERROR", 5);
			_delay_ms(500);
			lcd_locate(0,7);
			LCDstring(".", 1);
			_delay_ms(500);
			lcd_locate(0,8);
			LCDstring(".", 1);
			_delay_ms(500);
			lcd_locate(0,9);
			LCDstring(".", 1);
			_delay_ms(500);

			//flagi "wyzerowac"
			menuFlag = MENU_START;
			//UpDownFlag = 1;
			indicator = 1;
			displayMenu(menuFlag,indicator, 0);
		}
	}
}

void displayReadingMenu(int indicator) {
	char parametertab[3]; //nie wiem jaka wielkosc tablicy, przyjalem wartosc 10 to 3 elementowa bo jeszcze znak konca lini '\0'

	switch(indicator) {
		case 0: case 1: case 2: case 3:
		{
			lcd_locate(3,0);
			LCDstring("Temp: ", 6);		//3 - 7 bit LCD
			itoa(readingParameters.temp,parametertab,10);
			lcd_locate(9, 0);			//9 - 11
			LCDstring(parametertab,2); // bez znaku konca lini

			lcd_locate(3,1);
			LCDstring("Wilg: ", 6);		//3 - 7 bit LCD
			itoa(readingParameters.hum,parametertab,10);
			lcd_locate(9,1);			//9 - 11
			LCDstring(parametertab,2); // bez znaku konca lini

			lcd_locate(3,2);
			LCDstring("Cisn: ", 6);		//3 - 7 bit LCD
			itoa(readingParameters.press,parametertab,10);
			lcd_locate(9,2);			//9 - 11
			LCDstring(parametertab,2); // bez znaku konca lini

			lcd_locate(3,3);
			LCDstring("WG:  ", 4);		//3 - 7 bit LCD
			itoa(readingParameters.wg,parametertab,10);
			lcd_locate(9,3);			//9 - 11
			//if(readingParameters.wg <= 999)
			LCDstring(parametertab, strlen(parametertab)); // bez znaku konca lini
			lcd_locate(12,3);
			LCDstring("%", 1);
		}
		break;
		case 4:
		{
			lcd_locate(3,0);
			LCDstring("Wilg: ", 6);		//3 - 7 bit LCD
			itoa(readingParameters.hum,parametertab,10);
			lcd_locate(9,0);			//9 - 11
			LCDstring(parametertab,2); // bez znaku konca lini

			lcd_locate(3,1);
			LCDstring("Cisn: ", 6);		//3 - 7 bit LCD
			itoa(readingParameters.press,parametertab,10);
			lcd_locate(9,1);			//9 - 11
			LCDstring(parametertab,2); // bez znaku konca lini

			lcd_locate(3,2);
			LCDstring("WG: ", 4);		//3 - 7 bit LCD
			itoa(readingParameters.wg,parametertab,10);
			lcd_locate(9,2);			//9 - 11
			LCDstring(parametertab, strlen(parametertab)); // bez znaku konca lini
			lcd_locate(12,2);
			LCDstring("%", 1);

			lcd_locate(3,3);
			LCDstring("Nasl: ", 6);		//3 - 7 bit LCD
			itoa(readingParameters.foto,parametertab,10);
			lcd_locate(9,3);			//9 - 11
			LCDstring(parametertab, strlen(parametertab)); // bez znaku konca lini
			lcd_locate(12,3);
			LCDstring("%", 1);
		}
		break;
		case 5:
		{
			lcd_locate(3,0);
			LCDstring("Cisn: ", 6);		//3 - 7 bit LCD
			itoa(readingParameters.press,parametertab,10);
			lcd_locate(9,0);			//9 - 11
			LCDstring(parametertab,2); // bez znaku konca lini

			lcd_locate(3,1);
			LCDstring("WG: ", 4);		//3 - 7 bit LCD
			itoa(readingParameters.wg,parametertab,10);
			lcd_locate(9,1);			//9 - 11
			LCDstring(parametertab, strlen(parametertab)); // bez znaku konca lini
			lcd_locate(12,1);
			LCDstring("%", 1);

			lcd_locate(3,2);
			LCDstring("Nasl: ", 6);		//3 - 7 bit LCD
			itoa(readingParameters.foto, parametertab,10);
			lcd_locate(9,2);			//9 - 11
			LCDstring(parametertab, strlen(parametertab)); // bez znaku konca lini
			lcd_locate(12,2);
			LCDstring("%", 1);

			lcd_locate(3,3);
			LCDstring("Cofnij", 6);		//3 - 8 bit LCD
		}
		break;
		default:
		// co w przypadku bledu
		break;
	}
}

void displayThresholdsMenu(int indicator, int change) {
	char parametertab[3]; //nie wiem jaka wielkosc tablicy, przyjolem wartosc 10 to 3 elementowa bo jeszcze znak konca lini '\0'

	//display_indicator(indicator);

	switch(indicator)
	{
		case 0: case 1: case 2: case 3:
		{
			lcd_locate(3,0);
			LCDstring("Temp: ", 6);		//3 - 7 bit LCD

			lcd_locate(3,1);
			LCDstring("Wilg: ", 6);		//3 - 7 bit LCD

			lcd_locate(3,2);
			LCDstring("Cisn: ", 6);		//3 - 7 bit LCD

			lcd_locate(3,3);
			LCDstring("WG: ", 4);		//3 - 7 bit LCD
		}
		break;
		case 4:
		{
			lcd_locate(3,0);
			LCDstring("Wilg: ", 6);		//3 - 7 bit LCD

			lcd_locate(3,1);
			LCDstring("Cisn: ", 6);		//3 - 7 bit LCD

			lcd_locate(3,2);
			LCDstring("WG: ", 4);		//3 - 7 bit LCD

			lcd_locate(3,3);
			LCDstring("Nasl: ", 6);		//3 - 7 bit LCD
		}
		break;
		case 5:
		{
			lcd_locate(3,0);
			LCDstring("Cisn: ", 6);		//3 - 7 bit LCD

			lcd_locate(3,1);
			LCDstring("WG: ", 4);		//3 - 7 bit LCD

			lcd_locate(3,2);
			LCDstring("Nasl: ", 6);		//3 - 7 bit LCD

			lcd_locate(3,3);
			LCDstring("Hist: ", 6);		//3 - 7 bit LCD
		}
		break;
		case 6:
		{
			lcd_locate(3,0);
			LCDstring("WG: ", 4);		//3 - 7 bit LCD

			lcd_locate(3,1);
			LCDstring("Nasl: ", 6);		//3 - 7 bit LCD

			lcd_locate(3,2);
			LCDstring("Hist: ", 6);		//3 - 7 bit LCD

			lcd_locate(3,3);
			LCDstring("Cofnij", 6);		//3 - 8 bit LCD
		}
		break;
		default:
		// co w przypadku bledu
		break;
	}

	switch(indicator)
	{
		case 0:
		{
			thresholdsParameters.temp = thresholdsParameters.temp + change;
			itoa(thresholdsParameters.temp,parametertab,10);
			lcd_locate(9,0);
			LCDstring(parametertab, strlen(parametertab));		//8 - 10 bit LCD
		}break;
		case 1:
		{
			thresholdsParameters.hum = thresholdsParameters.hum + change;
			itoa(thresholdsParameters.hum,parametertab,10);
			lcd_locate(9,1);
			LCDstring(parametertab, strlen(parametertab));		//8 - 10 bit LCD
		}break;
		case 2:
		{
			thresholdsParameters.press = thresholdsParameters.press + change;
			itoa(thresholdsParameters.press,parametertab,10);
			lcd_locate(9,2);
			LCDstring(parametertab, strlen(parametertab));		//8 - 10 bit LCD
		}break;
		case 3:
		{
			thresholdsParameters.wg = thresholdsParameters.wg + change;
			itoa(thresholdsParameters.wg,parametertab,10);
			lcd_locate(9,3);
			LCDstring(parametertab, strlen(parametertab));		//8 - 10 bit LCD
		}break;
		case 4:
		{
			thresholdsParameters.foto = thresholdsParameters.foto + change;
			itoa(thresholdsParameters.foto,parametertab,10);
			lcd_locate(9,3);
			LCDstring(parametertab, strlen(parametertab));		//8 - 10 bit LCD
		}break;
		case 5:
		{
			thresholdsParameters.hysteresis = thresholdsParameters.hysteresis + change;
			itoa(thresholdsParameters.hysteresis,parametertab,10);
			lcd_locate(9,3);
			LCDstring(parametertab, strlen(parametertab));		//8 - 10 bit LCD
		}break;
		case 6:
		{
			//nic nie robic bo tu jest cofnij
		}break;
		default:
		//co w przypadku bledu
		break;
	}
}

void displayStartInformation(void)
{
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
	_delay_ms(2000);
	LCDclr();
}

