#include <xc.h>
__PROG_CONFIG(1,0x3FE4) ;     /* INTOSC, WDT off, MCLRE on, CLKOUT off        */
__PROG_CONFIG(2,0x1EFF) ;
#define _XTAL_FREQ 16000000

/* --- pip LEDs: anode <- R <- pin, cathode -> GND  => pin HIGH (1) = LED ON --- */
/* use the LAT latches for outputs to avoid read-modify-write pin hazards        */
#define LED1 LATAbits.LATA0
#define LED2 LATAbits.LATA1
#define LED3 LATAbits.LATA2
#define LED4 LATAbits.LATA3
#define LED5 LATAbits.LATA4
#define LED6 LATAbits.LATA5
#define LED7 LATAbits.LATA6
#define LED8 LATBbits.LATB0
#define LED9 LATBbits.LATB1

/* button: RA7 -> switch -> GND.  NEEDS AN EXTERNAL 10k PULL-UP TO VDD          */
/* (PIC16F1937 has NO weak pull-up on PORTA).  Pressed = RA7 low = BUTTON true. */
#define BUTTON (!PORTAbits.RA7)

/* common-anode 7-seg.  The real segment bit order on THIS board is             */
/* {a, b, c, d, e, g, f, dp}  (f and g are swapped vs. the textbook order).     */
/* These are the ORIGINAL values - they already produce correct digits 1..6.   */
unsigned char v[10] = { 0x03, 0x9F, 0x23, 0x0B, 0x99, 0x49, 0x41, 0x1F, 0x01, 0x09 };

unsigned char a;              /* Timer1 tick, ~25 ms (spare)                    */

void init (void);
void interrupt ia2 (void);

/* segments are split: RC0..RC5 carry 6 of them, RD6..RD7 carry the other 2.
   Do NOT disturb RC6/RC7 (they go to the ALIMENTARE / UART connector).        */
static void show_digit (unsigned char pat)
{
    LATC = (unsigned char)((LATC & 0b11000000) | (pat & 0b00111111));
    LATD = (unsigned char)((LATD & 0b00111111) | (pat & 0b11000000));
}

static void render (unsigned char c)
{
    switch (c) {
    case 0: case 6:  LED1=0;LED2=0;LED3=0;LED4=0;LED5=1;LED6=0;LED7=0;LED8=0;LED9=0; show_digit(v[1]); break; /* 1 */
    case 1:          LED1=1;LED2=0;LED3=0;LED4=0;LED5=0;LED6=0;LED7=0;LED8=0;LED9=1; show_digit(v[2]); break; /* 2 */
    case 7:          LED1=0;LED2=0;LED3=1;LED4=0;LED5=0;LED6=0;LED7=1;LED8=0;LED9=0; show_digit(v[2]); break; /* 2 */
    case 2:          LED1=1;LED2=0;LED3=0;LED4=0;LED5=1;LED6=0;LED7=0;LED8=0;LED9=1; show_digit(v[3]); break; /* 3 */
    case 8:          LED1=0;LED2=0;LED3=1;LED4=0;LED5=1;LED6=0;LED7=1;LED8=0;LED9=0; show_digit(v[3]); break; /* 3 */
    case 3: case 9:  LED1=1;LED2=0;LED3=1;LED4=0;LED5=0;LED6=0;LED7=1;LED8=0;LED9=1; show_digit(v[4]); break; /* 4 */
    case 4: case 10: LED1=1;LED2=0;LED3=1;LED4=0;LED5=1;LED6=0;LED7=1;LED8=0;LED9=1; show_digit(v[5]); break; /* 5 */
    case 5: case 11: LED1=1;LED2=0;LED3=1;LED4=1;LED5=0;LED6=1;LED7=1;LED8=0;LED9=1; show_digit(v[6]); break; /* 6 */
    }
}

void main (void)
{
    unsigned char c    = 0;   /* current frame 0..11                            */
    unsigned int  step = 0;   /* ms between frames while slowing; 0 = stopped   */
    unsigned int  t    = 0;   /* delay accumulator                             */

    init();

    while (1) {
        if (BUTTON) {                     /* held: roll fast                    */
            c++; if (c > 11) c = 0;
            step = 8;
            t = 0;
            __delay_ms(8);
        }
        else if (step) {                  /* released: slow down, then stop     */
            __delay_ms(1);
            if (++t >= step) {
                t = 0;
                c++; if (c > 11) c = 0;
                step += 3;                /* each frame a little slower         */
                if (step > 220) step = 0; /* die has settled                    */
            }
        }

        render(c);
    }
}

void init (void)
{
    OSCCON = 0x7B;                 /* 16 MHz HFINTOSC                           */

    TRISA  = 0b10000000;           /* RA7 = button input, RA0..RA6 = LED out    */
    ANSELA = 0x00;
    LATA   = 0x00;

    TRISB  = 0b00000000;           /* RB0/RB1 = LED8/LED9                       */
    ANSELB = 0x00;
    LATB   = 0x00;

    TRISC  = 0b11000000;           /* RC0..RC5 = segments; RC6/RC7 left alone   */
    LATC   = 0x00;

    TRISD  = 0b00000000;           /* RD6/RD7 = segments                        */
    LATD   = 0x00;

    T1CON  = 0x11;                 /* Fosc/4, 1:2, on  -> ~25 ms overflow       */
    TMR1IE = 1;
    PEIE   = 1;
    GIE    = 1;
}

void interrupt ia2 (void)
{
    if (TMR1IF) {
        TMR1IF = 0;
        TMR1H = 0x3C;
        TMR1L = 0xAF;
        a++;
    }
}
