
/**********************| Parallel port capture to 115200 serial |*******************


Reset             PC6

115200 baud 
  RX              PD0
  TX              PD1

18.432Mhz crystal
  XTAL1           PB6 
  XTAL2           PB7

HandShake
  /Strobe  (INT0) PD2
  Busy            PD3
  /ACK            PD4

Data lines
  D0              PC0
  D1              PC1
  D2              PC2
  D3              PC3
  D4              PC4
  D5              PC5
  D6              PB4
  D7              PB5
   
    
                       +---\/---+
        (/RESET)  PC6  |1     28|  PC5  (ADC5 / SCL)
           (RXD)  PD0  |2     27|  PC4  (ADC4 / SDA)
           (TXD)  PD1  |3     26|  PC3  (ADC3)
          (INT0)  PD2  |4     25|  PC2  (ADC2)
          (INT1)  PD3  |5     24|  PC1  (ADC1)
      (XCK / T0)  PD4  |6     23|  PC0  (ADC0)
                  VCC  |7     22|  GND
                  GND  |8     21|  AREF
 (XTAL1 / TOSC1)  PB6  |9     20|  AVCC
 (XTAL2 / TOSC2)  PB7  |10    19|  PB5  (SCK)          
            (T1)  PD5  |11    18|  PB4  (MISO)          
          (AIN0)  PD6  |12    17|  PB3  (MOSI / OC2)   
          (AIN1)  PD7  |13    16|  PB2  (SS / OC1B)    
          (ICP1)  PB0  |14    15|  PB1  (OC1A)         
                       +--------+

7   6   5   4   3   2   1   0
80  40  20  10  8   4   2   1

 
*************************************************************************/

#include <avr/io.h>
#include <stdint.h>
#include "avrcommon.h"
#include "usart.h"
#include <avr/interrupt.h>



// misc
#define OUTPUT              1
#define INPUT               0


#define SetBusy()   SetBit(3, PORTD) 
#define ClearBusy() ClearBit(3, PORTD) 

#define SetAck()    SetBit(4, PORTD) 
#define ClearAck()  ClearBit(4, PORTD) 


volatile uint8_t flag;
volatile uint8_t data;

void Delay(uint32_t v) ;

void INT0Setup() {         
  //MCUCR =  (1<<ISC00) | (1<<ISC01); // trigger interrupts on both edges

  MCUCR =  (1<<ISC01);                // trigger interrupt on falling edges
  GICR  =  (1<<INT0) ;                // then enable them.
}


ISR(INT0_vect){
 
 SetBusy();
 data  =  PINC & 0x3F;        // Oh give me a home, where contigious ports roam...
 data |= (PINB & 0x30) << 2;  
 flag = 1;
 
}


int main (void) {    
  	
  // set up directions               
  DDRB = (INPUT << PB0 | INPUT << PB1 |INPUT << PB2 |INPUT << PB3 | INPUT << PB4 |INPUT << PB5 |INPUT << PB6 |INPUT << PB7);       
  DDRC = (INPUT << PC0 | INPUT << PC1 |INPUT << PC2 |INPUT << PC3 | INPUT << PC4 |INPUT << PC5 |INPUT << PC6 );            
  DDRD = (INPUT << PD0 | INPUT << PD1 |INPUT << PD2 |OUTPUT << PD3 | OUTPUT << PD4 |INPUT << PD5 |INPUT << PD6 |INPUT << PD7);        

  USART_Init( 9 ); //setup serial  115200 fuseext     
  INT0Setup();
  sei();  
  flag = 0; 
  ClearBusy();
  SetAck();

  while(1){ 
    if (flag == 1) {           
      USART_Transmit( data );

      ClearAck(); 
      Delay(2);
      flag = 0;
      ClearBusy();
      SetAck();
      
    }
  }
} 


//=============================| Functions |==========================


void Delay(uint32_t v) {
 for(; v ; v--) NOP();
}


