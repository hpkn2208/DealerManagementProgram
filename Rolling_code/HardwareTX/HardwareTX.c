/*
 * SimulateTX.c
 *
 * Created: 4/29/2021 2:56:45 PM
 *  Author: Welcome
 */ 
// Code for Transmitter Model: FS1000A, Transmitting Frequency: 433.92MHz
#ifndef F_CPU
//define CPU clock speed if not defined
#define F_CPU 8000000UL // AVR ATMega328P 16MHz
#endif

#include <avr/io.h> // enable data flow control over pins
#include <util/delay.h> // enable delay function in program
#include <avr/interrupt.h>
//#include <avr/sleep.h>
#include <avr/eeprom.h>

//set desired baud rate

#define BAUD 300
#define MYUBRR F_CPU/16UL/BAUD-1
#define KEY_ADDRESS 0x00

//define communicate parameters
#define SYNC 0xAA // synchronization signal
#define RADDR 0x44 // receiver address

#define RED_TOGGLE 0x20// RED toggle command
#define BLUE_TOGGLE 0x22// BlUE toggle command
#define GREEN_TOGGLE 0x24// GREEN toggle command

#define BUTTON1 4 // button switch connected to port C pin 0
#define BUTTON2 2 //
#define BUTTON3 0 //
#define MODULE  0  // Power supply pin for module

#define DEBOUNCE_TIME 15 // time to wait while "de-bouncing" button
#define LOCK_INPUT_TIME 100 // time to wait after a button press

volatile uint8_t button_flag = 0;
volatile uint8_t button_flag1 = 0;

uint8_t count;
uint16_t key;
uint8_t b[256]={
	//0     1    2      3     4    5     6     7      8    9     A      B    C     D     E     F
	0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76,
	0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0,
	0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15,
	0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75,
	0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84,
	0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf,
	0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8,
	0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2,
	0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73,
	0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb,
	0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79,
	0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08,
	0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a,
	0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e,
	0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf,
	0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16 };

int encrypt(int count){// count is the counter to sync
	uint8_t	 x_box=b[count];			//Transmitter
	uint8_t	 y_box=b[x_box];
	uint16_t z_box=(count<<8)+y_box;
	return z_box;// Return Address 16bit
}

// Initializing UART
void usart_init(unsigned int ubrr)
{
	//set baudrate
	UBRR0H = (unsigned char)(ubrr>>8);
	UBRR0L = (unsigned char)ubrr;
	//enable receiver & transmitter
	UCSR0B = (1<<RXEN0)|(1<<TXEN0);
	//set frame 8N1
	UCSR0C = (0<<UMSEL01)|(0<<UMSEL00)|(0<<UPM01)|(0<<UPM00)|(0<<USBS0)|(0<<UCSZ02)|(1<<UCSZ01)|(1<<UCSZ00);
}

// Transmit data(byte) function
void USART_vSendByte(uint8_t u8Data){
	// Wait for transmit buffer to be empty
	while(!(UCSR0A&(1<<UDRE0)));
	// Transmit data - load data into transmit register
	UDR0 = u8Data;
}

// Send packet of data
void Send_Packet(uint8_t addr, uint8_t cmd, uint16_t key)
{
	uint8_t highbyte, lowbyte;
	highbyte =key>>8;
	lowbyte  =key&0xFF;
	USART_vSendByte(SYNC);//send synchronization byte
	USART_vSendByte(addr);//send receiver address
	USART_vSendByte(highbyte);
	USART_vSendByte(lowbyte);
	USART_vSendByte(cmd);//send increment command
	USART_vSendByte((addr+highbyte+lowbyte+cmd)&0x00FF);//send checksum
}

// Main initialization
void Main_Init(void)
{
	DDRB |= (1<<MODULE);
	PORTC = 0x00;
}

void extint_init(void){
	EICRA |= (1<<ISC00)|(1<<ISC10)|(1<<ISC11);
	EIMSK |= (1<<INT0)|(1<<INT1) ;
}
/*void sleep_init(void)
{
	set_sleep_mode(SLEEP_MODE_PWR_DOWN);
	sleep_enable();
	cli();
}*/
// main function - entry point of the program
int main(void)
{
	usart_init(MYUBRR);
	Main_Init();
	extint_init();
	//sleep_init();
	while(1)
	{
		if(button_flag == 1){
			if (bit_is_set(PINC,BUTTON1))// the button is pressed when BUTTON bit is clear
			{	PORTB |= (1<<MODULE);
				Send_Packet(RADDR, RED_TOGGLE,key);// check the state of the button again
				_delay_ms(LOCK_INPUT_TIME);button_flag1 = 1;}
			
			else if (bit_is_set(PINC,BUTTON2))// the button is pressed when BUTTON bit is clear
			{	PORTB |= (1<<MODULE);
				Send_Packet(RADDR, BLUE_TOGGLE,key);
				_delay_ms(LOCK_INPUT_TIME);	button_flag1 = 1;}
			
			else if (bit_is_set(PINC,BUTTON3))// the button is pressed when BUTTON bit is clear
			{	PORTB |= (1<<MODULE);
				Send_Packet(RADDR, GREEN_TOGGLE,key);
				_delay_ms(LOCK_INPUT_TIME);	button_flag1 = 1;}
			
			PORTB &= ~(1<<MODULE);
		}
		else{
			cli();
			button_flag1=0;
			//sleep_enable();
			//sleep_bod_disable();
			sei();
			//sleep_cpu();
			//sleep_disable();
		}
	}
	return 0;
}

ISR(INT0_vect){
	if(button_flag1) {button_flag = 0; count++;eeprom_update_byte((uint8_t*)KEY_ADDRESS,count);}
	else             {button_flag = 1;
					  count=eeprom_read_byte((uint8_t*)KEY_ADDRESS);
				      key=encrypt(count);
					  _delay_ms(DEBOUNCE_TIME);
					  }
}

ISR(INT1_vect){
	count = 150;eeprom_update_byte((uint8_t*)KEY_ADDRESS,count);
}