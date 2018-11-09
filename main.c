#include<msp430g2553.h>

/**
 * main.c
 */

void fadeLED(int dir);
unsigned int counter=0;
unsigned char Rx_Data = 0;                  // Byte received via UART
int distance=0;
int dist_inch=0;
int myA=0;


int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
    //config for utra sensor
    P2DIR=BIT3;
    P2DIR&=~BIT4;
    P1OUT=0;
    P2OUT=0;
    P2IE=BIT4;
    P2IES=BIT4;
    P2IFG=0;

    // config for bluetooth
    DCOCTL = 0;                             // Select lowest DCOx and MODx settings
    BCSCTL1 = CALBC1_1MHZ;                  // Set DCO
    DCOCTL = CALDCO_1MHZ;
    /*** Set-up GPIO ***/
    P1SEL = BIT1 + BIT2;                    // P1.1 = RXD, P1.2=TXD
    P1SEL2 = BIT1 + BIT2;                   // P1.1 = RXD, P1.2=TXD
    P1DIR |= BIT6 + BIT0;                   // P1.6 set as output
    P1OUT &= ~(BIT6 + BIT0);                // P1.6 set low
    /*** Set-up USCI A ***/
    UCA0CTL1 |= UCSSEL_2;                   // SMCLK
    UCA0BR0 = 104;                          // 1MHz 9600
    UCA0BR1 = 0;                            // 1MHz 9600
    UCA0MCTL = UCBRS0;                      // Modulation UCBRSx = 1
    UCA0CTL1 &= ~UCSWRST;                   // Initialize USCI state machine
    IE2 |= UCA0RXIE;                        // Enable USCI_A0 RX interrupt
    _enable_interrupts();                 //interrupts enabled

    //config motor
    P1DIR |= ( BIT4 | BIT5 | BIT6);             // set bits 4, 5, 6 as outputs
    P2DIR |= ( BIT1 | BIT2);             // set bit  0, 1 as outputs
    fadeLED(5);


    while(1)
    {
        myA=1;
        counter=0;
        P2OUT^=BIT3;
        __delay_cycles(10);
        P2OUT^=BIT3;
        //checkP2=0;

        while(counter<9000 && myA){
            counter+=1;
            //__delay_cycles(1);
            //checkP2=P2IN & BIT4;
        }
        if(counter<4000){
                distance=counter;
                dist_inch=(distance-12)/8;
                __delay_cycles(1);
            }
            else{
                distance=4000;
                dist_inch=1000;
                __delay_cycles(1);
            }


        switch (Rx_Data)
        {
            case 'F':                          // forward

                P1OUT &=~BIT4; //undo backward
                P1OUT |= BIT5; //puts motor forward
                P2OUT&=~BIT2;  // undo left
                P2OUT&=~BIT1;  // undo right

                /**
                //while (!(IFG2&UCA0TXIFG));    // USCI_A0 TX buffer ready?
                //UCA0TXBUF = 0x41;             // Send 8-bit character
                TA0CCTL0 &= ~CCIE;              // Disable Timer0_A interrupts
                P1SEL &= ~BIT6;                 // P1.6 selected as GPIO
                P1OUT |= BIT6 + BIT0;           // P1.0 and P1.6 set high
                */

                break;

            case 'B':

                P1OUT &=~BIT5; //undo forward
                P1OUT |= BIT4; //puts motor backward
                P2OUT&=~BIT2;  // undo left
                P2OUT&=~BIT1;  // undo right

                break;

            case 'L':

                P1OUT &=~BIT4; //undo backward
                P1OUT &=~BIT5; //undo forward
                P2OUT&=~BIT1;  // undo right
                P2OUT|=BIT2; // turns motor left
                break;

            case 'R':

                P1OUT &=~BIT4; //undo backward
                P1OUT &=~BIT5; //undo forward
                P2OUT&=~BIT2;  // undo left
                P2OUT|=BIT1; //turns motor right
                break;


            case 'S':
                P1OUT &=~BIT4; //undo backward
                P1OUT &=~BIT5; //undo forward
                P2OUT&=~BIT2;  // undo left
                P2OUT&=~BIT1;  // undo right
                break;



                /**
                // OFF Command
                //while (!(IFG2&UCA0TXIFG));    // USCI_A0 TX buffer ready?
                //UCA0TXBUF = 0x42;             // Send 8-bit character
                TA0CCTL0 &= ~CCIE;              // Disable Timer0_A interrupts
                P1SEL &= ~BIT6;                 // P1.6 selected as GPIO
                P1OUT &= ~(BIT6 + BIT0);        // P1.0 and P1.6 set low
                break;
                */
            case '0':                         // FLASH Command

                fadeLED(1);

                break;

            case '1':                         // FLASH Command

                fadeLED(2);

                break;

            case '2':                         // FLASH Command

                fadeLED(3);

                break;

            case '3':                         // FLASH Command

                fadeLED(4);

                break;

            case '4':                         // FLASH Command

                fadeLED(5);

                break;

            case '5':                         // FLASH Command

                fadeLED(6);

                break;

            case '6':                         // FLASH Command

                fadeLED(7);

                break;

            case '7':                         // FLASH Command

                fadeLED(8);

                break;

            case '8':                         // FLASH Command

                fadeLED(9);

                break;

            case '9':                         // FLASH Command

                fadeLED(10);

                break;

            default: break;
        }
        //__bis_SR_register(LPM0_bits);  // Enter LPM0, interrupts enabled



    }


}




#pragma vector=PORT2_VECTOR
__interrupt void PORT2_ISR(void){
    myA=0;

    P2IFG=0;
}

void fadeLED(int dir)
{
    int valuePWM = 100 - dir*3-70;
    P1SEL |= (BIT6);                              // P1.0 and P1.6 TA1/2 options
    CCR0 = 100 - 0;                               // PWM Period
    CCTL1 = OUTMOD_3;                             // CCR1 reset/set
    CCR1 = valuePWM;                              // CCR1 PWM duty cycle
    TACTL = TASSEL_2 + MC_1; // SMCLK, up mode
}


//  USCI A interrupt handler
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=USCIAB0RX_VECTOR
__interrupt void USCI0RX_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(USCIAB0RX_VECTOR))) USCI0RX_ISR (void)
#else
#error Compiler not supported!
#endif
{
    Rx_Data = UCA0RXBUF;                    // Assign received byte to Rx_Data
}

