
#include <avr/io.h>
#include "music.h"
#define NUM_MODES 2
#define F_CPU 16000000UL

unsigned char mode = 0;
char input = '\0';
unsigned char select = 0;

void USART_Init(unsigned long BAUDRATE);
void USART_TxChar(char data);
void Print_Serial(char* message, int length);
char USART_RxChar();
void Serial_Clear();

int main(void)
{
	DDRA = 0x00;
	PORTA = 0xFF;
	DDRE |= (1<<PORTE4);
	DDRE |= 0b00100000;	// PORTE bit 5 output for LED5
	DDRD = 0b11111011;	// PORTD output except bit 2 that is USART transmit & receive
	PORTD = 0xFF;		// turn off LEDs (active low)
	PORTE |= 0b00100000; // turn off center LED (active low)
	
	USART_Init(9600);
	
	unsigned char display_once = 0;
	unsigned char second_display = 0;

    while (1) 
    {
		//SW7 = PINA5 cycle through modes
		if((~PINA & (1 << PINA5)) == (1<<PINA5))
		{
			//Reset select var
			select = 0;
			mode++;
			mode = mode % NUM_MODES;
			display_once = 1;
			while((~PINA & (1 << PINA5)) == (1<<PINA5));
		}

		//SW8 = PINA6 cycle through modes
		if((~PINA & (1 << PINA6)) == (1<<PINA6))
		{
			//Select the item you are currently on denoted by the serial "<--"
			select = 1;
			second_display = 1;
		}
		
		// Check for keyboard input.
		input = USART_RxChar();
		while(input != '\0')
		{
			// If carriage return or new line were entered (console button turned on), ignore it.
			if((input == '\r') || (input == '\n'))
			{
				break;
			}
			
			// Determine which mode was entered.
			if (input == '1')
			{
				mode = 0;
				select = 1;
				second_display = 1;
			}
			else if (input == '2')
			{
				mode = 1;
				select = 1;
				second_display = 1;
			}
			else if (input == 'u')
			{
				char message[]= "Increasing the Octave!";
				Print_Serial(message, 22);
				increase_octave();
			}
			else if(input == 'd')
			{
				char message[]= "Decreasing the Octave!";
				Print_Serial(message, 22);
				decrease_octave();
			}
			else
			{
				// Print not found message to the screen.
				char notFound[] = "Input does not match anything that we are expecting!\n";
				Print_Serial(notFound,54);
			}
			
			// Grab the next input from the keyboard.
			input = USART_RxChar();
		}
		
		// Execute the current mode.
		switch(mode)
		{
			case 0:
				if (display_once)
				{
					//Clear Terminal
					Serial_Clear();
					// Print mode message to the screen.
					char modeMessage[] = "Short Tune Mode <--\nPiano Mode";
					Print_Serial(modeMessage,30);
					
					display_once = 0;
				}
				if (select){
					if(second_display){
						//Clear Terminal
						Serial_Clear();
						// Print mode message to the screen.
						char modeMessage[] = "You are now in Short Tune Mode!";
						Print_Serial(modeMessage,31);
						display_titles();
						second_display = 0;
					}
					short_tunes();
				}
				break;
			case 1:
				if (display_once)
				{
					//Clear Terminal
					Serial_Clear();
					// Print mode message to the screen.
					char modeMessage[] = "Short Tune Mode\nPiano Mode <--";
					Print_Serial(modeMessage,30);
					display_once = 0;
				}
				if (select == 1){
					if(second_display){
						//Clear Terminal
						Serial_Clear();
						// Print mode message to the screen.
						char modeMessage[] = "You are now in Piano Mode!";
						Print_Serial(modeMessage,26);
						second_display = 0;
					}
					spontaneous_tunes();
				}
				break;
		}
    }
}

//Using newline characters; flush previous messages of the serial terminal
void Serial_Clear()
{
	// Push everything off page.
	char modeMessage[] = "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n";
	Print_Serial(modeMessage,60);
}

//Print to the terminal serially
void Print_Serial(char* message, int length)
{
	for (int i = 0; i < length; i++)
	{
		USART_TxChar(message[i]);
	}
	USART_TxChar('\n');
}


void USART_Init(unsigned long BAUDRATE)			/* USART initialize function */
{
	UCSR1B |= (1<<RXEN) | (1<<TXEN);

	UBRR1 = (F_CPU / (16 * BAUDRATE)) - 1;
}


// Data receiving function    (hint pg.227)
char USART_RxChar()
{
	// checks to see if there is a character to receive
	// if so, it returns it
	// if not, it returns a null
	
	if (UCSR1A & (1<<RXC))
	{
		return UDR1;
	} else
	{
		return '\0';
	}
}


// Data transmitting function (hint pg.224)
void USART_TxChar(char data)
{
	// Write data to be transmitting in UDR
	// Wait until data transmit and buffer get empty
	while( !(UCSR1A & (1<<UDRE)));
	UDR1 = data;
}
