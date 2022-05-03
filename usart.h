// AVR306: Using the AVR UART in C
// Routines for polled USART
// Last modified: 02-06-21
// Modified by: AR

/* Includes */
#include <avr/io.h>
#include <stdio.h>
#include <stdarg.h>

#define HAVEDATA (UCSRA & (1<<RXC))

/* Prototypes */
void USART_Init( unsigned int baudrate );
unsigned char USART_Receive( void );
void USART_Transmit( unsigned char data );
void USART_printhex(uint8_t i);
void USART_printstring(unsigned char *data);

