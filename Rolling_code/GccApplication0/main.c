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

void ADC_Init()
{
    ADCSRA = 0x87;          /* Enable ADC, fr/128  */
    ADMUX = 0x40;           /* Vref: Avcc, ADC channel: 0 */

}

int ADC_Read(char channel)
{

     int Ain,AinLow;

     ADMUX=ADMUX|(channel & 0x0f);  /* Set input channel to read */

     ADCSRA |= (1<<ADSC);           /* Start conversion */
     while((ADCSRA&(1<<ADIF))==0);  /* Monitor end of conversion interrupt */

     _delay_us(10);
     AinLow = (int)ADCL;                /* Read lower byte*/
     Ain = (int)ADCH*256;           /* Read higher 2 bits and 
                                    Multiply with weight */
    Ain = Ain + AinLow;             
    return(Ain);                    /* Return digital value*/
}

int main(void)
{
	LCD_Dir = 0xFF;
	LCD_Init();
	ADC_Init();
	LCD_String("Lux Meter LDR:");
	LCD_goto_xy(1,0);
	LCD_String("Intensity:    lx");
	while(1)
	{
		LCD_goto_xy(1,11);
		LCD_String("");
	}
}