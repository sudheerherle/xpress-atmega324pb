
#include <avr/io.h>
#include "music.h"

void USART_TxChar(char data);
void Print_Serial(char* message, int length);
void Display_Bun();
void Display_Mario();
void Serial_Clear();

//octave value is used to modify the length of the delay
//used in notes, so will pitch the tunes/piano notes up or down
//based on its value (0.5, 1, 1.5, 2)
float octave = 1;
char notes_played[1000];
int num_of_notes = 0;

void decrease_octave()
{
	octave += 0.5;
	//If octave goes too high (> 2), reset to 0.5 (can't have octave = 0, causes issues with timer
	//and with division by 0)
	octave = (octave == 2.5 ? 0.5 : octave);
}

void increase_octave()
{
	octave -= 0.5;
	//If octave goes too low (<= 0), reset to 2 (can't have negative octave shift)
	octave = (octave == 0 ? 2 : octave);
}

void my_delay_1ms()
{
	TCCR1A = 0x00;
	TCCR1B = 0b00001011;	//sets counter in CTC mode, 64 prescaler
	
	TCNT1 = 0x00;			//counts up to OCR1A
	OCR1A = 0xFA * octave;	//multiply by octave to increase/decrease timer delay proportionate to the octave
	
	while((TIFR1 & (1<<OCF1A)) == 0x00)	//TIFR0 contains overflow flag (for when counter finishes)
	{
	}
	
	TIFR1 |= (1<<OCF1A);
	
	TCCR1A = 0;
	TCCR1B = 0;
}

void my_delay_01ms()
{
	TCCR1A = 0x00;
	TCCR1B = 0b00001011;	//sets counter in CTC mode, 64 prescaler
	
	TCNT1 = 0x00;		//counts up to OCR1A
	OCR1A = 0x19 * octave;		//multiply by octave to increase/decrease timer delay proportionate to the octave
	
	while((TIFR1 & (1<<OCF1A)) == 0x00)	//TIFR0 contains overflow flag (for when counter finishes)
	{
	}
	
	TIFR1 |= (1<<OCF1A);
	
	TCCR1A = 0;
	TCCR1B = 0;
}

void my_delay_ms(int ms)
{
	for (int i = 0; i < ms; i++)
	{
		my_delay_1ms();
	}
}

void display_titles()
{
	char title1[] = "1. Mario";
	char title2[] = "2. Hot Cross Buns";
	Print_Serial(title1,8);
	Print_Serial(title2,17);
}

//This function monitors several switches and will play the tunes if SW1 or SW2 is pressed
//If SW3 is pressed, it will increase the octave for all notes played
void short_tunes()
{
	if ((~PINA & (1 << PINA0)) == (1<<PINA0))
	{
		tune1();
	}
	
	if ((~PINA & (1 << PINA1)) == (1<<PINA1))
	{
		tune2();
	}
	
	//increase octave
	if ((~PINA & (1 << PINA2)) == (1<<PINA2))
	{
		increase_octave();
	}
}
//Plays three different spontaneous notes when buttons are pressed
void spontaneous_tunes()
{
	if ((~PINA & (1 << PINA0)) == (1<<PINA0))
	{
		notes_played[num_of_notes] = 'A';
		USART_TxChar('A');
		play_A(100);
		num_of_notes++;
	}
	
	if ((~PINA & (1 << PINA1)) == (1<<PINA1))
	{
		notes_played[num_of_notes] = 'B';
		USART_TxChar('B');
		play_B(100);
		num_of_notes++;
	}
	if ((~PINA & (1 << PINA2)) == (1<<PINA2))
	{
		notes_played[num_of_notes] = 'C';
		USART_TxChar('C');
		play_C(100);
		num_of_notes++;
	}
	// Increase octave
	if ((~PINA & (1 << PINA3)) == (1<<PINA3))
	{
		increase_octave();
	}
	// Displays and plays notes played so far
	if ((~PINA & (1 << PINA4)) == (1<<PINA4))
	{
		USART_TxChar('\n');
		for(int i = 0; i < num_of_notes; i++)
		{
			if(notes_played[i] == 'A')
			{
				USART_TxChar('A');
				play_A(100);
				my_delay_ms(100);
			}
			if(notes_played[i] == 'B')
			{
				USART_TxChar('B');
				play_B(100);
				my_delay_ms(100);
			}
			if(notes_played[i] == 'C')
			{
				USART_TxChar('C');
				play_C(100);
				my_delay_ms(100);
			}
		}
		USART_TxChar('\n');
	}
}

//Plays mario theme song (the timing is not exactly correct)
void tune1()
{
	Display_Mario();
	play_E(100);
	my_delay_ms(5);
	play_E(100);
	my_delay_ms(10);
	play_E(100);
	my_delay_ms(5);
	play_C(90);
	my_delay_ms(5);
	play_E(120);
	my_delay_ms(10);
	play_G(150);
	my_delay_ms(40);
	decrease_octave();
	play_G(100);
	my_delay_ms(10);
	play_C(100);
	my_delay_ms(10);
	play_G(100);
	my_delay_ms(10);
	play_E(100);
	my_delay_ms(10);
	play_A(75);
	my_delay_ms(5);
	play_B(75);
	my_delay_ms(5);
	play_Bb(75);
	my_delay_ms(5);
	play_A(75);
	my_delay_ms(5);
	play_G(75);
	increase_octave();
	my_delay_ms(5);
	play_E(100);
	my_delay_ms(5);
	play_G(100);
	Serial_Clear();
}

//Plays hot cross buns
void tune2()
{
	Display_Bun();
	play_B(150);
	my_delay_ms(5);
	play_A(175);
	my_delay_ms(5);
	play_G(300);
	my_delay_ms(5);

	play_B(150);
	my_delay_ms(5);
	play_A(175);
	my_delay_ms(5);
	play_G(300);
	my_delay_ms(5);
	
	play_G(100);
	my_delay_ms(5);
	play_G(100);
	my_delay_ms(5);
	play_G(100);
	my_delay_ms(5);
	play_G(100);
	my_delay_ms(5);

	play_A(100);
	my_delay_ms(5);
	play_A(100);
	my_delay_ms(5);
	play_A(100);
	my_delay_ms(5);
	play_A(100);
	my_delay_ms(5);

	play_B(150);
	my_delay_ms(5);
	play_A(150);
	my_delay_ms(5);
	play_G(300);
}

//plays (what should be) the note C over the piezo speaker
void play_C(int length)
{
	PORTD &= 0b11111110; // turn on LED1
	for (int i = 0; i < length / octave; i++)
	{
		//Toggle the speaker port pin at a particular frequency to create this note
		PORTE ^= (1<<PORTE4);
		//Should have a 3.8ms delay for a C note
		my_delay_1ms();
		my_delay_1ms();
		my_delay_1ms();
		my_delay_01ms();
		my_delay_01ms();
		my_delay_01ms();
		my_delay_01ms();
		my_delay_01ms();
		my_delay_01ms();
		my_delay_01ms();
		my_delay_01ms();
	}
	PORTD |= ~0b11111110; // turn off LED1
}

void play_D(int length)
{
	PORTD &= 0b11111101; // turn on LED2
	for (int i = 0; i < length / octave; i++)
	{
		PORTE ^= (1<<PORTE4);
		my_delay_1ms();
		my_delay_1ms();
		my_delay_1ms();
		my_delay_01ms();
		my_delay_01ms();
		my_delay_01ms();
		my_delay_01ms();
	}
	PORTD |= ~0b11111101; // turn off LED2
}

void play_E(int length)
{
	PORTE &= 0b11011111; // turn on LED5
	for (int i = 0; i < length / octave; i++)
	{
		PORTE ^= (1<<PORTE4);
		my_delay_1ms();
		my_delay_1ms();
		my_delay_1ms();
	}
	PORTE |= ~0b11011111; // turn off LED5
}

void play_F(int length)
{
	PORTD &= 0b11101111; // turn on LED6
	for (int i = 0; i < length / octave; i++)
	{
		PORTE ^= (1<<PORTE4);
		my_delay_1ms();
		my_delay_1ms();
		my_delay_01ms();
		my_delay_01ms();
		my_delay_01ms();
		my_delay_01ms();
		my_delay_01ms();
		my_delay_01ms();
		my_delay_01ms();
		my_delay_01ms();
		my_delay_01ms();
	}
	PORTD |= ~0b11101111; // turn off LED6
}

void play_G(int length)
{
	PORTD &= 0b11011111; // turn on LED7
	for (int i = 0; i < length / octave; i++)
	{
		PORTE ^= (1<<PORTE4);
		my_delay_1ms();
		my_delay_1ms();
		my_delay_01ms();
		my_delay_01ms();
		my_delay_01ms();
		my_delay_01ms();
		my_delay_01ms();
		my_delay_01ms();
	}
	PORTD |= ~0b11011111; // turn off LED7
}

void play_A(int length)
{
	PORTD &= 0b10111111; // turn on LED8
	for (int i = 0; i < length / octave; i++) // divide by octave to account for delay increase/decrease so note is the length when the same length is passed in with different octaves
	{
		PORTE ^= (1<<PORTE4);
		my_delay_1ms();
		my_delay_1ms();
		my_delay_01ms();
		my_delay_01ms();
		my_delay_01ms();
	}
	PORTD |= ~0b10111111; // turn off LED8
}

void play_Bb(int length) // Same as A#
{
	PORTD &= 0b01111111; // turn on LED9
	for (int i = 0; i < length / octave; i++)
	{
		PORTE ^= (1<<PORTE4);
		my_delay_1ms();
		my_delay_1ms();
		my_delay_01ms();
	}
	PORTD |= ~0b01111111; // turn off LED9
}

void play_B(int length)
{
	PORTD &= 0b01111111; // turn on LED9
	for (int i = 0; i < length / octave; i++)
	{
		PORTE ^= (1<<PORTE4);
		my_delay_1ms();
		my_delay_1ms();
	}
	PORTD |= ~0b01111111; // turn off LED9
}

//Display the Bun ASCII art
void Display_Bun(){

	//Strings containing the ASCII art as one line
	char bun_image_1[]="        %1        \n       /*/   /%1  \n       1**1  1**1 \n       |**|  |**| \n       /**  /**/  \n       */   */    \n       _____      \n     %* / / *%    \n   &*  / /    *&  \n  %*   ``      *% \n  |             | \n   1...   ___../  \n      ```          \n";
	//Length of the art string
    unsigned int len_bun_1=249;
		
	//Clear for better view of art
	Serial_Clear();

	//This is what handles rendering the proper image
	Print_Serial(bun_image_1,len_bun_1);
	//Print the title of the song once more
	char title[] = "Now Playing... Hot Cross Buns";
	Print_Serial(title,30);
}

//Display the Mario ASCII art
void Display_Mario(){

	//Strings containing the ASCII art as one line
	char mario_image_1[]="     __________\n    |**********|..\n   |****|  |0| |__\n   |***|          ]\n    `|    ` |***|\n  ---[------]---\n |    |*|_|*|    |\n |   ||*****||   |\n|   _||*0**0*|_   |\n |_|  |******| |_|\n    _|**|`|**|_\n   |    | |    |\n";

	//Length of the art string
    unsigned int len_mario_1=218;
		
	//Clear for better view of art
	Serial_Clear();
	Print_Serial(mario_image_1,len_mario_1);
	//Print the title of the song once more
	char title[] = "Now Playing... Mario";
	Print_Serial(title,21);
}
