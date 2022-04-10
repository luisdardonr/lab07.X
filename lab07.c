/* 
 * File:   lab07.c
 * Author: Luis Dardon 
 *
 * Created on 6 de abril de 2022, 04:43 PM
 */
#pragma config FOSC = INTRC_NOCLKOUT// Oscillator Selection bits (INTOSCIO oscillator: I/O function on RA6/OSC2/CLKOUT pin, I/O function on RA7/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = OFF      // RE3/MCLR pin function select bit (RE3/MCLR pin function is digital input, MCLR internally tied to VDD)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = OFF      // Brown Out Reset Selection bits (BOR disabled)
#pragma config IESO = OFF       // Internal External Switchover bit (Internal/External Switchover mode is disabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is disabled)
#pragma config LVP = OFF        // Low Voltage Programming Enable bit (RB3 pin has digital I/O, HV on MCLR must be used for programming)

// CONFIG2
#pragma config BOR4V = BOR40V   // Brown-out Reset Selection bit (Brown-out Reset set to 4.0V)
#pragma config WRT = OFF        // Flash Program Memory Self Write Enable bits (Write protection off)

#include <xc.h>
#include <stdint.h>  // Para poder usar los int de 8 bits
#include <stdio.h>

#define aumen PORTBbits.RB0
#define decre PORTBbits.RB1

#define _XTAL_FREQ 4000000

#define tmr0r 250

uint8_t acumulador;
uint8_t conta;

int band;
int unidad;
int decena;
int centena;
int display;
int sobr;
void setup(void);

char tabla[10] = {0b00111111, 0b00000110, 0b01011011, 0b01001111, 0b01100110, 
0b01101101, 0b01111101, 0b00000111, 0b01111111, 0b01101111};

void setup(void){
    ANSEL = 0;
    ANSELH = 0;
    
    TRISA = 0x00;               // PORTA como salida
    PORTA = 0;                  // Limpiamos PORTA
    TRISC = 0; //out
    TRISD = 0; // out
    PORTC = 0;
    
    
    TRISBbits.TRISB0 = 1;
    TRISBbits.TRISB1 = 1;
    OPTION_REGbits.nRBPU = 0;   // Habilitamos resistencias de pull-up del PORTB
    WPUBbits.WPUB0 = 1;         // Habilitamos resistencia de pull-up de RB0
    WPUBbits.WPUB1 = 1;         // Habilitamos resistencia de pull-up de RB0
    
    INTCONbits.GIE = 1;         // Habilitamos interrupciones globales
    INTCONbits.RBIE = 1;        // Habilitamos interrupciones del PORTB
    IOCBbits.IOCB0 = 1;         // Habilitamos interrupción por cambio de estado para RB0
    IOCBbits.IOCB1 = 1;         // Habilitamos interrupción por cambio de estado para RB0
    INTCONbits.RBIF = 0;        // Limpiamos bandera de interrupción
    
    OPTION_REGbits.T0CS = 0;    // UTILIZAR CICLO INTERNO
    OPTION_REGbits.PSA = 0;     // CAMBIAR PRESCALER A TMR0
    OPTION_REGbits.PS0 = 1;     // COLOCAR PRESCALER EN 1:256
    OPTION_REGbits.PS1 = 1;     // 
    OPTION_REGbits.PS2 = 1;     // 
    
    INTCONbits.T0IF = 0;        // LIMPIAR BANDERA DE INTERRUPCION EN TMR0
    TMR0 = tmr0r;            // VALOR DE TMR0
    
    INTCONbits.T0IE = 1;        // HABILITAR INTERRUPCIONES DE TMR0
    INTCONbits.T0IF = 0;        // LIMPIAR BANDERA DE INTERRUPCION EN TMR0
    
    
    OSCCONbits.IRCF = 0b0110;   // 4MHz
    OSCCONbits.SCS = 1;         // Oscilador interno

}

void __interrupt() isr(void){
    
    if(INTCONbits.RBIF){
        if (!aumen){             // REVISAR SI RB0 FUE PRESIONADO
            PORTA++;            // INCREMENTAR PORTA
            display = PORTA;
        }
        else if(!decre){         // REVISAR SI RB1 FUE PRESIONADO
            PORTA--;            // DECREMENTAR PORTA
            display = PORTA;
        }
        INTCONbits.RBIF = 0;    // LIMPIAR BANDERA DE INTERRUPCION EN PORTB
    }
    else if(T0IF){
        INTCONbits.T0IF = 0;    
        TMR0 = tmr0r;
    
        PORTD = 0; 
        if (band == 0) //display 0
        {
            PORTC = (tabla[unidad]);
            PORTDbits.RD2 = 0;
            PORTDbits.RD0 = 1;
            
            band = 1;
        }
        else if (band == 1) //display 1
        {
            PORTC = (tabla[decena]);
            PORTDbits.RD0 = 0;
            PORTDbits.RD1 = 1;
            
            band = 2;
        }
        else if (band == 2) //display 2
        {
            PORTC = (tabla[centena]);
            PORTDbits.RD1 = 0;
            PORTDbits.RD2 = 1;
            
            band = 0;
        }
         
    
    }
    return;
}

int mostrar(void)
{
    sobr = display%100;
    unidad = sobr%10;
    decena = sobr/10;
    centena = display/100;
}
void main(void){
    setup(); 
    while(1){
        
        mostrar();
    }
    return;
}

