#include <xc.h>
__PROG_CONFIG(1,0x3FE4) ;
__PROG_CONFIG(2,0x1EFF) ;
#define _XTAL_FREQ 16000000

#define LED1 RA0
#define LED2 RA1
#define LED3 RA2
#define LED4 RA3
#define LED5 RA4
#define LED6 RA5
#define LED7 RA6
#define LED8 RB0
#define LED9 RB1

#define BUTTON !RA7

unsigned char v[10] = { 0x03, 0x9F, 0x25, 0x0D,0x99,0x49,0x41,0x1f,0x01,0x09};
unsigned char a;

void init (void);
void interrupt ia2(void);


void main(void) {

   init();

// "counter" is the variable that stores the current number that is displayed on the dice
static unsigned char c = 0;

// this variable is used for the "slowing down" effect
static int b= 0;

  while(1) {
    if (BUTTON)
       {
      c++;
      if (c > 11)
     {
          c = 0;

         }
      b= 1500;
     PORTA = 0b10000000;
        }
    else
    {
        if (b > 0)
       {
            b--;
            if (b % 100 == 0)
           {
                c++;
                if (c > 11)
           {
                    c= 0;

                }
            }
        }
    }

    if (c== 0) {
        LED1 = 0;
        LED2 = 0;
        LED3 = 0;
        LED4 = 0;
        LED5 = 1;
        LED6 = 0;
        LED7 = 0;
        LED8 = 0;
        LED9 = 0;
       PORTC=v[1];
       PORTD=v[1];

    // 2
    } else if (c == 1) {
        LED1 = 1;
        LED2 = 0;
        LED3 = 0;
        LED4 = 0;
        LED5 = 0;
        LED6 = 0;
        LED7 = 0;
        LED8 = 0;
        LED9 = 1;
        PORTC=v[2];
       PORTD=v[2];


    // 3
    } else if (c == 2) {
        LED1 = 1;
        LED2 = 0;
        LED3 = 0;
        LED4 = 0;
        LED5 = 1;
        LED6 = 0;
        LED7 = 0;
        LED8 = 0;
        LED9 = 1;
       PORTC=v[3];
       PORTD=v[3];

    // 4
    } else if (c == 3) {
        LED1 = 1;
        LED2 = 0;
        LED3 = 1;
        LED4 = 0;
        LED5 = 0;
        LED6 = 0;
        LED7 = 1;
        LED8 = 0;
        LED9 = 1;
       PORTC=v[4];
       PORTD=v[4];

    // 5
    } else if (c == 4) {
        LED1 = 1;
        LED2 = 0;
        LED3 = 1;
        LED4 = 0;
        LED5 = 1;
        LED6 = 0;
        LED7 = 1;
        LED8 = 0;
        LED9 = 1;
       PORTC=v[5];
       PORTD=v[5];

    // 6
    } else if (c == 5) {
        LED1 = 1;
        LED2 = 0;
        LED3 = 1;
        LED4 = 1;
        LED5 = 0;
        LED6 = 1;
        LED7 = 1;
        LED8 = 0;
        LED9 = 1;
       PORTC=v[6];
       PORTD=v[6];

    // 1
    } else if (c == 6) {
        LED1 = 0;
        LED2 = 0;
        LED3 = 0;
        LED4 = 0;
        LED5 = 1;
        LED6 = 0;
        LED7 = 0;
        LED8 = 0;
        LED9 = 0;
       PORTC=v[1];
       PORTD=v[1];

    // 2
    } else if (c == 7) {
        LED1 = 0;
        LED2 = 0;
        LED3 = 1;
        LED4 = 0;
        LED5 = 0;
        LED6 = 0;
        LED7 = 1;
        LED8 = 0;
        LED9 = 0;
       PORTC=v[2];
       PORTD=v[2];

    // 3
    } else if (c == 8) {
        LED1 = 0;
        LED2 = 0;
        LED3 = 1;
        LED4 = 0;
        LED5 = 1;
        LED6 = 0;
        LED7 = 1;
        LED8 = 0;
        LED9 = 0;
       PORTC=v[3];
       PORTD=v[3];

    // 4
    } else if (c == 9) {
        LED1 = 1;
        LED2 = 0;
        LED3 = 1;
        LED4 = 0;
        LED5 = 0;
        LED6 = 0;
        LED7 = 1;
        LED8 = 0;
        LED9 = 1;
       PORTC=v[4];
       PORTD=v[4];

    // 5
    } else if (c == 10) {
        LED1 = 1;
        LED2 = 0;
        LED3 = 1;
        LED4 = 0;
        LED5 = 1;
        LED6 = 0;
        LED7 = 1;
        LED8 = 0;
        LED9 = 1;
       PORTC=v[5];
       PORTD=v[5];

    // 6
    } else if (c== 11) {
        LED1 = 1;
        LED2 = 1;
        LED3 = 1;
        LED4 = 0;
        LED5 = 0;
        LED6 = 0;
        LED7 = 1;
        LED8 = 1;
        LED9 = 1;
       PORTC=v[6];
       PORTD=v[6];
    }

  }

}

void init (void)
{
OSCCON = 0x7B;

TRISA = 0x00;
ANSELA = 0b00000000;
PORTA = 0b10000000;

TRISD = 0b00000000;

PORTD= 0b00000000;

TRISB = 0b00000000;
ANSELB = 0b00000000;
PORTB = 0b00000000;

 TRISC = 0x00;
PORTC= 0x00;

T1CON = 0x11;

TMR1IE = 1;
PEIE = 1;
GIE = 1;
}
void interrupt ia2(void)
{
if(TMR1IF)
 {
 TMR1IF = 0;
 TMR1H = 0x3C;
 TMR1L = 0xAF;
 a++;
 }
}
