/*
 * stop_watch_project.c
 *
 *  Created on: Sep 21, 2021
 *      Author: Hayat Hussein
 */


#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
unsigned char tick=0;
unsigned char second = 0;
unsigned char minute = 0;
unsigned char hour = 0;
//enum FLAG{paused,resume};
unsigned char sec_first_digit;		//will use them to display each digit
unsigned char sec_second_digit;
unsigned char min_first_digit;
unsigned char min_second_digit;
unsigned char hours_first_digit;
unsigned char hours_second_digit;
//unsigned char flag=resume;		/*USED TO STOP AND RESUME THE CLOCK*/

#define SET_BIT(REG,BIT) (REG|=(1<<BIT))
#define CLEAR_BIT(REG,BIT) (REG&=(~(1<<BIT))
void TIMER1_Init(void )
{

	TCCR1A = (1<<FOC1A);		/*non pwm mode*/
	TCCR1B = (1<<WGM12)|(1<<CS02);	/*CTC Mode -prescaler 256*/
	TCNT1 = 0;
	OCR1A = 3906;
	SET_BIT(TIMSK,OCIE1A);// ENABLE Timer1 Compare match interrupt
}
void RESET_intr0_init(void)
{
	SET_BIT(MCUCR,ISC01);	/*intrru 0 generated on falling edge*/
	MCUCR&=~(1<<ISC00);
	GICR|=(1<<INT0);		/*ENABLE INTR 0*/

	DDRD&=~(1<<PD2);	//INPUT PIN push button AND INT0 IS HERE
	PORTD|=(1<<PD2);	//INTERNAL PULL UP RESESTANCE
}
void pause_intr1_init(void)
{
	DDRD&=~(1<<PD3);
	MCUCR|=(1<<ISC11)|(1<<ISC10);		/*intrru 1 generated on rising edge*/
	GICR|=(1<<INT1);	/*enable int1*/

}
void resume_intr2_init(void){
		MCUCSR&=~(1<<ISC2);		/*FALLING EDGE TRIGGERED INTR2*/
		GICR|=(1<<INT2);		/*enable intrubt 2*/
		DDRB&=~(1<<PB2);	//INPUT PIN push button AND INT2 IS HERE
		PORTB|=(1<<PB2);	//INTERNAL PULL UP RESESTANCE

}
ISR(TIMER1_COMPA_vect)
{
	second ++;
	if (second == 60){
		second = 0;
		minute ++;
	}
	if (minute == 60){
		second = 0;
		minute = 0;
		hour ++;
	}
	if (hour == 24){
		second = 0;
		minute = 0;
		hour = 0 ;
	}
	sec_first_digit=second % 10;
	sec_second_digit=second / 10;

	min_first_digit=minute%10;
	min_second_digit=minute/10;

	hours_first_digit=hour%10;
	hours_second_digit=hour/10;

}

ISR(INT0_vect){
	hour=0;
	second=0;
	minute=0;
}



ISR(INT1_vect)
{
	TCCR1B&=~(1<<CS10);		//turn off the clock so no timer
	TCCR1B&=~(1<<CS11);
	TCCR1B&=~(1<<CS12);
}



ISR(INT2_vect){
	TCCR1B = (1<<WGM12)|(1<<CS02);	/*CTC Mode -prescaler 256start the clock again with 265 prescaler*/

}
int main()
{
	DDRA = 0xff;	/*port A IS ALL Output*/
	PORTA = 0xff;	/*ALL 7 SEGMENTS ARE INITIALLY ENABLED*/
	DDRC = 0x0f;	//configure the 7seg data pins to output
	PORTC = 0x00;  // at beginning all LEDS off
	SREG|=(1<<7); //enable global intrubts
	TIMER1_Init( );		//START TIMER1
	RESET_intr0_init();
	pause_intr1_init();
	resume_intr2_init();
	while(1)
	{

		PORTA = (1<<0);		/**enable the 7 segment that we want*/
		PORTC = sec_first_digit;		/*then display the correct digit*/
		_delay_ms(3);			/*VERY small delay so that we dont see the leds turning on and off*/
		PORTA = (1<<1);
		PORTC =sec_second_digit ;
		_delay_ms(3);
		PORTA = (1<<2);
		PORTC =min_first_digit;
		_delay_ms(3);
		PORTA = (1<<3);
		PORTC = min_second_digit;
		_delay_ms(3);
		PORTA = (1<<4);
		PORTC = hours_first_digit;
		_delay_ms(3);
		PORTA = (1<<5);
		PORTC = hours_second_digit;
		_delay_ms(3);
	}


}

