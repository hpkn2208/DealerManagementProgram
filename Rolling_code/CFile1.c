#ifndef F_CPU
//define CPU clock speed if not defined
#define F_CPU 16000000UL // AVR ATMega328P 16MHz
#endif

#include <avr/io.h> // enable data flow control over pins
#include <avr/interrupt.h> // enable interrupt function in program
#include <util/delay.h> // enable delay function in program
#include <avr/eeprom.h>

#define LCD_Dir  DDRB			/* Define LCD data port direction */
#define LCD_Port PORTB			/* Define LCD data port */
#define RS PB1				/* Define Register Select pin */
#define RW PB2
#define EN PB3 				/* Define Enable signal pin */

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

int main(void)
{
	LCD_Dir = 0xFF;
	LCD_Init();
	LCD_String("Working on...");
	LCD_goto_xy(1,0);
	LCD_String("Baudrate: ");
	while(1)
	{
		
	}
}