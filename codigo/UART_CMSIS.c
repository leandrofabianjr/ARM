#include "msp432p401r.h"

int main(void) {
	// Desliga watchdog timer
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;

	// Desbloqueia o modulo CS para acesso aos registradores
	CS->KEY = 0x695A;
	// Reseta parametros
	CS->CTL0 = 0;
	// DCO em 12 MHz (nominal, centro entre 8 MHz e 16 MHz)
	CS->CTL0 = CS_CTL0_DCORSEL_3;
	// ACLK = REFO, SMCLK = MCLK = DCO
	CS->CTL1 = CS_CTL1_SELA_2 | CS_CTL1_SELS_3 | CS_CTL1_SELM_3;
	// Desbloqueia o modulo CS (registradores nao sao mais alterados)
	CS->KEY = 0;

	// Configura pinos UART como TX = P1.3 e RX = P1.2
	P1->SEL0 |= BIT2 | BIT3;

	// Habilita interrupcoes no processador
	__enable_interrupt();
	// Habilita interrupcao do eUSCIA0 no vetor de interrupcoes (NVIC)
	NVIC->ISER[0] = 1 << ((EUSCIA0_IRQn) & 31);

	// Habilita softare reset (desliga o eUSCI)
	EUSCI_A0->CTLW0 |= EUSCI_A_CTLW0_SWRST;
	// Fonte de clock da UART como SMCLK
	EUSCI_A0->CTLW0 |= EUSCI_B_CTLW0_SSEL__SMCLK;

	// Calculo do baud rate
	// UCxxBRx = clock/(16 * baud rate)
	// 12000000/(16*9600) = 78.125
	UCA0BR0 = 78;
	UCA0BR1 = 0x00;
	// Porcao fracional = 0.125
	// Tabela 22-4 no User Guide: UCBRSx = 0x10
	// UCBRFx = int ( (78.125-78)*16) = 2
	EUSCI_A0->MCTLW = 0x1000 | EUSCI_A_MCTLW_OS16 | 0x0020;

	// Desabilita software reset (inicializa o eUSCI)
	EUSCI_A0->CTLW0 &= ~EUSCI_A_CTLW0_SWRST;
	// Habilita interrupcao do RX da USCI_A0
	EUSCI_A0->IE |= EUSCI_A_IE_RXIE;

	// Superlaco
	while (1) {
		// Modo de economia de energia LPM3
		__sleep();
		// Informa ao debugger que esta em modo de economia
		__no_operation();
	}
}

// Rotina de interupcao da UART
void EUSCIA0_IRQHandler(void) {
	// Se houve interrupcao e foi de recepcao (RX)
	if (EUSCI_A0->IFG & EUSCI_A_IFG_RXIFG) {
		// Espera ate haver transmissao
		while (!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG));
		// Buffer de transmissao recebe o que ha no de recepcao
		EUSCI_A0->TXBUF = EUSCI_A0->RXBUF;

		// Informa ao debugger que esta em modo de economia
		__no_operation();
	}
}