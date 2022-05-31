/*=======================================================
					SENAI A. Jacob Lafer
					====================

Nome do projeto:	Timer e M�quina de estados
Vers�o:				1.0x
�ltima revis�o:		08/06/2020
Cliente:			SENAI
Desenvolvimento:	Luiz Ricardo Bitencourt

Descritivo Funcional:
Exemplo de uso de Timer0 com uma m�quina de estados finitos.
A ideia � manter alguns recursos funcionando de forma independente
da m�quina.

Inputs:
- RB0 e RB1 - Chaves
- RC2 e RC4 - Chaves
 
Outputs:
- RD0-RD3 - LEDs
- RB2 - Display RS
- RB3 - Display E
- RB4-RB7 - Display Data
=========================================================*/

//======================================================
// Bibliotecas
//======================================================
#include <xc.h>
#include "lcd_4vias.h"
#include "timer0.h"

// =====================================================
// CONFIGURA��ES DO MICROCONTROLADOR
// =====================================================
#pragma config  FOSC    = HS
#pragma config  PLLDIV  = 1
#pragma config  CPUDIV  = OSC1_PLL2
#pragma config  USBDIV  = 1
#pragma config  BORV    = 0
#pragma config  WDTPS   = 1
#pragma config  MCLRE   = ON
#pragma config	PWRT=ON, LVP=OFF, IESO=OFF, WDT=OFF, PBADEN=OFF, BOR=OFF
#pragma config  CCP2MX=ON, LPT1OSC=OFF, ICPRT=OFF, XINST=OFF, VREGEN=OFF
#pragma config  CP0=OFF, CP1=OFF, CP2=OFF, CP3=OFF, CPB=OFF, CPD=OFF, WRTD = OFF
#pragma config  WRT0=OFF, WRT1=OFF, WRT2=OFF, WRT3=OFF, WRTC = OFF, WRTB = OFF
#pragma config  EBTR0=OFF, EBTR1=OFF, EBTR2=OFF, EBTR3=OFF, EBTRB=OFF
// =====================================================

//======================================================
// Defini��es de Hardware
//======================================================
#define LED1    PORTDbits.RD0
#define LED2    PORTDbits.RD1
#define LED3    PORTDbits.RD2
#define LED4    PORTDbits.RD3
#define LED8    PORTDbits.RD7

#define Botao01 PORTCbits.RC4
#define Botao02 PORTCbits.RC2
#define Botao03 PORTBbits.RB0
#define Botao04 PORTBbits.RB1
//======================================================

//======================================================
// Declara��o de Vari�veis
//======================================================
char    Estado = 1;             // Vari�vel usada na m�quina de estados
int		Cronometro  = 0,
        Segundos	= 0,
        Minutos     = 0,
        Horas       = 0,
		Preset		= 0xc2f7;  // Per�odo de 1s
//======================================================

//======================================================
// Interrup��es
//======================================================
void __interrupt() high_isr(void) {     // Tratamento de interrup��o de alta prioridade (ISR)
    if (INTCONbits.TMR0IF) {            // Verifica se overflow TMR0IF = 1 (garante que n�o executar� sem ter estourado a contagem)
        INTCONbits.TMR0IF = 0;          // Zera flag de overflow
                                        // Recarga do preset do Timer 0:
        TMR0H = Preset >> 8;            // Registrador de contagem do Timer 0 - Parte alta
        TMR0L = Preset;                 // Registrador de contagem do Timer 0 - Parte baixa

        Cronometro++;
        Segundos++;
        if (Segundos == 60) {
            Segundos = 0;
            Minutos++;
            if (Minutos == 60) {
                Minutos = 0;
                Horas++;
                if (Horas == 24) {
                    Horas = 0;
                }
            }
        }
    }
}
//======================================================

//======================================================
// Programa Principal
//======================================================
void main (void){
    // -------------------------------------------------
	// Cria��o de vari�veis
	// -------------------------------------------------
    int Tempo = 0;
    // -------------------------------------------------
    
	// -------------------------------------------------
	// Configura��es iniciais
	// -------------------------------------------------
	TRISD = 0x00;           // Todos os pinos do PortD como sa�das
    TRISB = 0b00000011;     // B0 e B1 s�o entradas, demais s�o sa�das
    TRISCbits.TRISC2 = 1;   // Pino RC2 configurado como entrada
    //TRISCbits.RC4 = 1;    // Apenas para mostrar que o pino RC4 n�o � configurado como entrada desta forma no 18F4550
                            // RC4 � ativada pelas op��es de USB abaixo.
    UCON = 0x00;            //** PARA O PIC 18F4550: desativa op��es de USB para utilizar a porta RC4 como entrada
    UCFGbits.UTRDIS = 1;    //** PARA O PIC 18F4550: desativa op��es de USB para utilizar a porta RC4 como entrada
    
	IniciaLCD();            // Inicializa LCD
    Inicializa_Timer0(Timer0_INT_ON, Preset);	// Inicializa Timer0 com interrup��o e Preset (vide vari�vel no in�cio do c�digo)
    
    PORTD = 0x0f;           // Apaga todos os LEDs
	// -------------------------------------------------

	// -------------------------------------------------
	// Programa principal
	// -------------------------------------------------
   	PosicionaLCD(1,1);
    StringLCD("   SENAI  118   ");
    PosicionaLCD(2,1);
    StringLCD("Estado:         ");            
    
    Tempo = Cronometro;     // Verifica valor atual do Cron�metro
    
    // -------------------------------------------------
    // In�cio da estrutura da m�quina de estados
    // -------------------------------------------------
    while(1){
        // -------------------
        // A��es independentes (acontecem sempre):
        // -------------------
        if(Cronometro >= Tempo+5){      // Verifica se passou 5s
            LED8 = ~LED8;               // Inverte o estado l�gico do LED8
            Tempo = Cronometro;         // Atualiza valor para nova contagem
        }
        // -------------------
        
        switch(Estado){
            case 1:
                // -------------------
                // A��es do estado 1:
                // -------------------
                LED1 = 1;   // Liga LED1
                LED2 = 0;   // Desliga LED2
                LED3 = 0;   // Desliga LED3
                LED4 = 0;   // Desliga LED4
                PosicionaLCD(2,9);
                EscreveLCD(Estado%10 + 48);     // Escreve valor num�rico do estado
                // -------------------
                // Condi��o de transi��o:
                // -------------------
                if(Botao01 == 1){   // Se for pressionado o Botao01
                    Estado = 2;     // pula para o estado 2
                }
                // -------------------
                break;
                
            case 2:
                // -------------------
                // A��es do estado 2:
                // -------------------
                LED1 = 0;   // Desliga LED1
                LED2 = 1;   // Liga LED2
                LED3 = 0;   // Desliga LED3
                LED4 = 0;   // Desliga LED4
                PosicionaLCD(2,9);
                EscreveLCD(Estado%10 + 48);     // Escreve valor num�rico do estado
                // -------------------
                // Condi��o de transi��o:
                // -------------------
                if(Botao02 == 1){   // Se for pressionado o Botao02
                    Estado = 3;     // pula para o estado 3
                }
                // -------------------
                break;
                
            case 3:
                // -------------------
                // A��es do estado 3:
                // -------------------
                LED1 = 0;   // Desliga LED1
                LED2 = 0;   // Desliga LED2
                LED3 = 1;   // Liga LED3
                LED4 = 0;   // Desliga LED4
                PosicionaLCD(2,9);
                EscreveLCD(Estado%10 + 48);     // Escreve valor num�rico do estado
                // -------------------
                // Condi��o de transi��o:
                // -------------------
                if(Botao03 == 1){   // Se for pressionado o Botao03
                    Estado = 4;     // pula para o estado 4
                }
                // -------------------
                break;
                
            case 4:
                // -------------------
                // A��es do estado 4:
                // -------------------
                LED1 = 0;   // Desliga LED1
                LED2 = 0;   // Desliga LED2
                LED3 = 0;   // Desliga LED3
                LED4 = 1;   // Liga LED4
                PosicionaLCD(2,9);
                EscreveLCD(Estado%10 + 48);     // Escreve valor num�rico do estado
                // -------------------
                // Condi��o de transi��o:
                // -------------------
                if(Botao04 == 1){   // Se for pressionado o Botao04
                    Estado = 1;     // pula para o estado 1
                }
                // -------------------
                break;
        }
    }
    // Fim da estrutura da m�quina de estados
	// -------------------------------------------------
}
//======================================================

