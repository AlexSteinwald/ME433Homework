#include<xc.h> // processor SFR definitions
#include<sys/attribs.h> // __ISR macro

// DEVCFGs here
// DEVCFG0
#pragma config DEBUG = OFF // no debugging
#pragma config JTAGEN = OFF // no jtag
#pragma config ICESEL = ICS_PGx1 // use PGED1 and PGEC1
#pragma config PWP = OFF // no write protect
#pragma config BWP = OFF // not boot write protect
#pragma config CP = OFF // no code protect

// DEVCFG1
#pragma config FNOSC = PRIPLL // use primary oscillator with pll
#pragma config FSOSCEN = OFF // turn off secondary oscillator
#pragma config IESO = OFF // no switching clocks
#pragma config POSCMOD = HS // high speed crystal mode
#pragma config OSCIOFNC = OFF // free up secondary osc pins by turning sosc off  **?????
#pragma config FPBDIV = DIV_1 // divide CPU freq by 1 for peripheral bus clock
#pragma config FCKSM = CSDCMD // do not enable clock switch
#pragma config WDTPS = PS1 // slowest wdt
#pragma config WINDIS = OFF // no wdt window
#pragma config FWDTEN = OFF // wdt off by default
#pragma config FWDTWINSZ = WINSZ_25 // wdt window at 25%

// DEVCFG2 - get the CPU clock to 40MHz
#pragma config FPLLIDIV = DIV_2 // divide input clock to be in range 4-5MHz **DIVIDE BY 2 = 4
#pragma config FPLLMUL = MUL_20 // multiply clock after FPLLIDIV **MULTIPLY BY 20 = 80
#pragma config FPLLODIV = DIV_2 // divide clock after FPLLMUL to get 40MHz **DIVIDE BY 2 = 40
#pragma config UPLLIDIV = DIV_2 // divide 8MHz input clock, then multiply by 12 to get 48MHz for USB **divide by 2
#pragma config UPLLEN = ON // USB clock on

// DEVCFG3
#pragma config USERID = 0 // some 16bit userid, doesn't matter what
#pragma config PMDL1WAY = ON // allow only one reconfiguration
#pragma config IOL1WAY = ON // allow only one reconfiguration
#pragma config FUSBIDIO = ON // USB pins controlled by USB module
#pragma config FVBUSONIO = ON // controlled by USB module
//End DEVCFGx

int readADC(void);

int main() {

    // startup
__builtin_disable_interrupts();

// set the CP0 CONFIG register to indicate that
// kseg0 is cacheable (0x3) or uncacheable (0x2)    **IS cacheable???
// see Chapter 2 "CPU for Devices with M4K Core"
// of the PIC32 reference manual
__builtin_mtc0(_CP0_CONFIG, _CP0_CONFIG_SELECT, 0xa4210583);

// no cache on this chip!

// 0 data RAM access wait states
BMXCONbits.BMXWSDRM = 0x0;

// enable multi vector interrupts
INTCONbits.MVEC = 0x1;

// disable JTAG to be able to use TDI, TDO, TCK, TMS as digital
DDPCONbits.JTAGEN = 0;

__builtin_enable_interrupts();
//END STARTUP

    // set up USER pin as input **USER = B13
ANSELBbits.ANSB13 = 0; //Make B13 Digital
//NOT NEEDED: U1RXRbits.U1RXR = 0b0011; //Makes RPB13 an input
//TRISBbits.TRISB13 = 1; //B13 (24) is USER

    // set up LED1 pin as a digital output **LED1 = B7
TRISBbits.TRISB7 = 0;
// NOT NEEDED: RPB7Rbits.RPB7R = 0b001; //Set B7 as output U1TX

    // set up LED2 as OC1 using Timer2 at 1kHz **LED2 = B15
RPB15Rbits.RPB15R = 0b0101; //Set B15 as OC1
T2CONbits.TCKPS = 0; //Prescalar = 1
PR2 = 39999; //Max counter value
TMR2 = 0; // Initialize timer 2
OC1CONbits.OCM = 0b110; //Without failsafe
OC1RS = 2000; //Duts cycle is 5%
OC1R = 2000; //Initial duty cycle is 5%
T2CONbits.ON = 1; //turns timer on
OC1CONbits.ON = 1; //Turns output control on

    // set up A0 as AN0
    ANSELAbits.ANSA0 = 1;
    AD1CON3bits.ADCS = 3;
    AD1CHSbits.CH0SA = 0;
    AD1CON1bits.ADON = 1;

    while (1) {
        // invert pin every 0.5s, set PWM duty cycle % to the pot voltage output %
        _CP0_SET_COUNT(0); // set core timer to 0, remember it counts at half the CPU clock
    LATBINV = 0b10000000; // invert a pin B7 (LED1)

    // wait for half a second, setting LED brightness to pot angle while waiting
    while (_CP0_GET_COUNT() < 10000000) {
        int val;
        val = readADC();
        OC1RS = val * 39; //comes out to about 39

        if (PORTBbits.RB13 == 1) {
            // nothing
        } else {
            LATBINV = 0b10000000; //Invert LED1
        }
    }
    }
}

int readADC(void) {
    int elapsed = 0;
    int finishtime = 0;
    int sampletime = 20;
    int a = 0;

    AD1CON1bits.SAMP = 1;
    elapsed = _CP0_GET_COUNT();
    finishtime = elapsed + sampletime;
    while (_CP0_GET_COUNT() < finishtime) {
    }
    AD1CON1bits.SAMP = 0;
    while (!AD1CON1bits.DONE) {
    }
    a = ADC1BUF0;
    return a;
}
