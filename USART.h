/*
 * USART.h
 *
 * Created: 24.05.2022 09:47:39
 *  Author: piotr
 */ 

#define FOSC 16000000UL // Clock Speed
#define BAUD 9600
#define MYUBRR (((FOSC / (BAUD*8UL)))-1)

void USART_Init(void)
{
	//Set baud rate
	UBRR0H = (unsigned char)(MYUBRR>>8);
	UBRR0L = (unsigned char)MYUBRR;
	
	//double speed
	UCSR0A = (1<<U2X0);
	//Enable receiver and transmitter
	UCSR0B = (1<<RXEN0) | (1<<TXEN0) | (1<<RXCIE0);
	// Set frame format: 8data, 1stop bit
	UCSR0C = (3<<UCSZ00);
}

unsigned char USART_Receive( void )
{
	/* Wait for data to be received */
	while ( !(UCSR0A & (1<<RXC0)) );
	
	/* Get and return received data from buffer */
	return UDR0;
}


void UART_TxChar(char ch)
{
	while (! (UCSR0A & (1<<UDRE0)));	/* Wait for empty transmit buffer*/
	UDR0 = ch ;
}

void UART_SendString(char *str)
{
	unsigned char j=0;
	
	while (str[j]!=0)		/* Send string till null */
	{
		UART_TxChar(str[j]);
		j++;
	}
}