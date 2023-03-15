/*
 * SimulateRX.c
 *
 * Created: 5/14/2021 5:11:42 PM
 *  Author: Welcome
 */ 

#ifndef F_CPU
//define CPU clock speed if not defined
#define F_CPU 8000000UL // AVR ATMega328P 16MHz
#endif

#include <stdio.h>
#include <avr/io.h> // enable data flow control over pins
#include <avr/interrupt.h> // enable interrupt function in program
#include <util/delay.h> // enable delay function in program
#include <avr/eeprom.h>
#include <avr/sfr_defs.h>

//set desired baud rate

#define BAUD 1200
#define MYUBRR F_CPU/16UL/BAUD-1

//define receive parameters
#define SYNC 0xAA // synchronization signal
#define RADDR 0x44 // receiver address
#define KEY_ADDRESS 0x00FF	// key address	

#define UP 0x20// RED toggle command
#define HOLD 0x22// BlUE toggle command
#define DOWN 0x24// GREEN toggle command

#define ABOVE 3
#define BELOW 2

#define LED1	2 // Led1 connected to port C pin 0
#define LED2	4 // Led2 connected to port C pin 1
#define LED3	5 // Led3 connected to port C pin 2
#define BUZZER	0

#define BUT1 5 // Led1 connected to port C pin 0
#define BUT2 6 // Led2 connected to port C pin 1
#define BUT3 7 // Led3 connected to port C pin 2

#define LCD_Dir  DDRB			/* Define LCD data port direction */
#define LCD_Port PORTB			/* Define LCD data port */
#define RS PB1				/* Define Register Select pin */
#define RW PB2
#define EN PB3 				/* Define Enable signal pin */

uint16_t count;
uint8_t a[256]={
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

uint8_t b[256]={
	//0     1    2      3     4    5     6     7      8    9     A      B    C     D     E     F
	0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76,
	0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x86, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72,
	0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xd9, 0xea, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8,
	0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x68, 0x05, 0x9a, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75,
	0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84,
	0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf,
	0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8,
	0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2,
	0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73,
	0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb,
	0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79,
	0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0x65, 0x7a, 0xae, 0x08, 0x15,
	0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a,
	0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0xc1, 0x1d, 0x9e, 0xc0,
	0xe1, 0xf8, 0x98, 0x11, 0x69, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf, 0x31,
0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16, 0x07 };

int decrypt(uint8_t temp_1, uint8_t temp_2 , uint8_t temp_3, uint8_t temp_4, uint8_t count_predict){
	//Receiver
	
	uint16_t window_size= 32;
	uint8_t   count1=(temp_1&0xf0)+(temp_2&0x0f);
	uint8_t   count2=(temp_3&0xf0)+(temp_4&0x0f);
	uint8_t   y_box1=(temp_1&0x0f)+(temp_2&0xf0);
	uint8_t   y_box2=(temp_3&0x0f)+(temp_4&0xf0);
	uint16_t temp_count= (count1<<8)+count2;
	
	if (count_predict<65535-window_size){
		if((temp_count>=count_predict)&&(temp_count<count_predict+window_size)){
			if( (a[a[count1]]==y_box1) && (b[b[count2]]==y_box2) ){
				return 1;
			}else return 0;
		}
	}
	
	else if (count_predict>=65535-window_size){
		if((temp_count>=count_predict)||(temp_count<(count_predict+window_size))){
			if( (a[a[count1]]==y_box1) && (b[b[count2]]==y_box2) ){
				return 1;
			}else return 0;
		}
	}else return 0;
	return 0;
}
// Initializing UART
void usart_init(unsigned int ubrr)
{
	//set baudrate
	UBRR0H = (unsigned char)(ubrr>>8);
	UBRR0L = (unsigned char)ubrr;
	//enable receiver & transmitter
	UCSR0B=(1<<RXEN0);
	//set frame 8N1
	UCSR0C = (0<<UMSEL01)|(0<<UMSEL00)|(0<<UPM01)|(0<<UPM00)|(0<<USBS0)|(0<<UCSZ02)|(1<<UCSZ01)|(1<<UCSZ00);
}

// Read the value out of the UART buffer
uint8_t USART_vReceiveByte(void)
{
	// Wait until a byte has been received
	while((UCSR0A&(1<<RXC0)) == 0);
	// Return received data
	return UDR0;
}

void LCD_Command( unsigned char cmnd )
{
	LCD_Port = (LCD_Port & 0x0F) | (cmnd & 0xF0); /* sending upper nibble */
	LCD_Port &= ~ (1<<RS);		/* RS=0, command reg. */
	LCD_Port &= ~ (1<<RW);		// RW = 0
	LCD_Port |= (1<<EN);		/* Enable pulse */
	_delay_us(1);
	LCD_Port &= ~ (1<<EN);

	_delay_us(200);

	LCD_Port = (LCD_Port & 0x0F) | (cmnd << 4);  /* sending lower nibble */
	LCD_Port |= (1<<EN);
	_delay_us(1);
	LCD_Port &= ~ (1<<EN);
	_delay_ms(2);
}

void LCD_Char( unsigned char data )
{
	LCD_Port = (LCD_Port & 0x0F) | (data & 0xF0); /* sending upper nibble */
	LCD_Port |= (1<<RS);		/* RS=1, data reg. */
	LCD_Port &= ~ (1<<RW);		// RW = 0
	LCD_Port|= (1<<EN);
	_delay_us(1);
	LCD_Port &= ~ (1<<EN);

	_delay_us(200);

	LCD_Port = (LCD_Port & 0x0F) | (data << 4); /* sending lower nibble */
	LCD_Port |= (1<<EN);
	_delay_us(1);
	LCD_Port &= ~ (1<<EN);
	_delay_ms(2);
}

void LCD_String (char *str)		/* Send string to LCD function */
{
	int i;
	for(i=0;str[i]!=0;i++)		/* Send each char of string till the NULL */
	{
		LCD_Char (str[i]);
	}
}

void HextoASCII(unsigned char hex)
{	
	unsigned char temp_L;           // dummy variable that will hold LSBs of Hex number
	unsigned char temp_H;           // dummy variable that will hold MSBs of Hex number
	 temp_H = hex & 0xF0;           // Obtain the upper 4 bits (MSBs) of hex number
	 temp_H = temp_H >> 4;           //
	if(temp_H >9)                   // Check if the number is a letter
	  temp_H += 0x37;             // Convert the number to a letter in ASCII
	else
		temp_H += 0x30;             // Convert the number to ASCII number
	
	LCD_Char(temp_H);           // Display the number

	temp_L = hex & 0x0F;           // Obtain the lower 4 bits (LSBs) of hex number
	if(temp_L >9)                   // Check if the the number is a letter
		temp_L += 0x37;             // Convert the number to a letter in ASCII
	else
		temp_L += 0x30;             // Convert the number to ASCII number
	LCD_Char(temp_L);           // Display the number
}

void LCD_goto_xy (char row, char col)	/* Send string to LCD with xy position */
{
	if (row == 0 && col<16)
	LCD_Command((col & 0x0F)|0x80);	/* Command of first row and required position<16 */
	else if (row == 1 && col<16)
	LCD_Command((col & 0x0F)|0xC0);	/* Command of first row and required position<16 */
}

void LCD_Clear()
{
	LCD_Command (0x01);		/* Clear display */
	_delay_ms(2);
	LCD_Command (0x80);		/* Cursor at home position */
}

void up(void){
	if(!(PINC&(1<<ABOVE)))	PORTC=0x30;
	else					PORTC=0x22;
}

void stop(void){
	PORTC=0x22;
}

void down(void){
	if(!(PINC&(1<<BELOW)))	PORTC=0x12;
	else					PORTC=0x22;
}

void extint_init(void){
	EICRA |= (1<<ISC00)|(1<<ISC01)|(1<<ISC11)|(1<<ISC10);
	EIMSK |= (1<<INT0)|(1<<INT1);
}

void LCD_Init (void)			/* LCD Initialize function */
{
	_delay_ms(20);			/* LCD Power ON delay always >15ms */
	LCD_Command(0x02);		/* send for 4 bit initialization of LCD  */
	LCD_Command(0x28);              /* 2 line, 5*7 matrix in 4-bit mode */
	LCD_Command(0x0c);              /* Display on cursor off*/
	LCD_Command(0x06);              /* Increment cursor (shift cursor to right)*/
	LCD_Command(0x01);              /* Clear display screen*/
	_delay_ms(200);
}

// Main initialization
void Main_Init(void)
{
	DDRC=0xFFu;		//	Set all pins of the PORTC as output.
	stop();
	LCD_Dir = 0xFF;	// Make LCD port direction as o/p 
	count=eeprom_read_word((uint16_t*)KEY_ADDRESS);
	sei();			//enable global interrupts
}

// main function - entry point of the program
int main(void)
{
	usart_init(MYUBRR);
	extint_init();
	Main_Init();
	LCD_Init();
	LCD_String("Working on...");
	char baudrate[4];
	sprintf(baudrate,"%d",BAUD);
	LCD_goto_xy(1,0);
	LCD_String("Baudrate: ");
	LCD_String(baudrate);
	// main endless loop
	while(1)
	{	
		//define variables
		uint8_t raddress, temp_1, temp_2,temp_3,temp_4, data, chk;
		// receive destination address
		raddress= USART_vReceiveByte();
		// receive higbyte
		temp_1= USART_vReceiveByte();
		// receive lowbyte
		temp_2= USART_vReceiveByte();
		// receive higbyte
		temp_3= USART_vReceiveByte();
		// receive lowbyte
		temp_4= USART_vReceiveByte();
		// receive data
		data   = USART_vReceiveByte();
		// receive checksum
		chk    = USART_vReceiveByte();
	if (raddress==RADDR)
	{
		// compare received checksum with calculated
		if(chk==((raddress+temp_1+temp_2+temp_3+temp_4+data)&0x00FF))//if match perform operations
		{
			if(decrypt(temp_1,temp_2,temp_3,temp_4,count))
			{	
				PORTB|=(1<<BUZZER);//buzzer on
				uint8_t   count1=(temp_1&0xf0)+(temp_2&0x0f);
				uint8_t   count2=(temp_3&0xf0)+(temp_4&0x0f);
				uint16_t  current_count= (count1<<8)+ count2;
				LCD_Clear();
				LCD_String("C:");
				HextoASCII(raddress);
				HextoASCII(temp_1);
				HextoASCII(temp_2);
				HextoASCII(temp_3);
				HextoASCII(temp_4);
				HextoASCII(data);
				HextoASCII(chk);
				LCD_goto_xy(1,0);
				LCD_String("Key:");
				HextoASCII(count1);
				HextoASCII(count2);
				LCD_String(" Cmd: ");
				HextoASCII(data);
				if     (data==UP  )	  up();
				else if(data==HOLD )	stop();
				else if(data==DOWN)	down();
				count = current_count;
				eeprom_write_word((uint16_t*)KEY_ADDRESS,count);
			}
		}
	}
		PORTB&=~(1<<BUZZER);//buzzer of
		
	}
	return 0;
}

ISR(INT1_vect){
	if      (bit_is_set(PIND,BUT1))	up();
	else if ((bit_is_set(PIND,BUT2))||(bit_is_set(PINC,ABOVE))||(bit_is_set(PINC,BELOW))) stop();
	else if (bit_is_set(PIND,BUT3)) down();
	else;
}

ISR(INT0_vect){
	count = 150;
}