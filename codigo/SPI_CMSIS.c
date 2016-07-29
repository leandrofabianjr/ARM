#include "msp432p401r.h"

// Byte de recepcao
static uint8_t RXData = 0;
// Byte de transmissao
static uint8_t TXData;

int main(void) {
	volatile uint32_t i;

	// Desliga watchdog timer
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;

	// Configura pinos da SPI
	// P9.7 = DataIn
	// P9.6 = DataOut
	// P9.5 = Clock
	P9->SEL0 |= BIT5 | BIT6 | BIT7;          // set 3-SPI pin as second function
	// Habilita interrupcoes no processador
	__enable_interrupt();
	// Habilita interrupcao do eUSCIA3 no vetor de interrupcoes (NVIC)
	NVIC->ISER[0] = 1 << ((EUSCIA3_IRQn) & 31);

	// Habilita softare reset (desliga o eUSCI)
	EUSCI_A3->CTLW0 |= EUSCI_A_CTLW0_SWRST;
	// SPI mestre de 3 pinos e 8 bytes
	EUSCI_A3->CTLW0 |= EUSCI_B_CTLW0_MST | EUSCI_B_CTLW0_SYNC
			| EUSCI_A_CTLW0_CKPL | EUSCI_B_CTLW0_MSB;
	// ACLK como fonte de clock da SPI
	EUSCI_A3->CTLW0 |= EUSCI_B_CTLW0_SSEL__ACLK;
	// ACLK / 2, fBitClock = fBRCLK/(UCBRx+1)
	UCA3BR0 = 0x01;
	UCA3BR1 = 0;
	// Sem modulacao
	EUSCI_A3->MCTLW = 0;
	// Desabilita software reset (inicializa o eUSCI)
	EUSCI_A3->CTLW0 &= ~EUSCI_A_CTLW0_SWRST;

	// Inicializa dado a ser transmitido
	TXData = 0x01;
	// Acorda ao sair de uma rotina de tratamento de interrupcao
	SCB->SCR &= ~SCB_SCR_SLEEPONEXIT_Msk;

	// Superlaco
	while (1) {
		// Habilita interrupcao de transmissao da SPI
		EUSCI_A3->IE |= EUSCI_A__TXIE;
		// Modo de economia de energia LPM3
		__sleep();
		// Informa ao debugger que esta em modo de economia
		__no_operation();

		// Delay entre transmissoes
		for (i = 2000; i > 0; i--);
		// Incrementa dado a ser trasmitido
		TXData++;
	}
}

// Rotina de tratamenrto de interrupcao da SPI
void EUSCIA3_IRQHandler(void) {
	// Se houve uma interrupcao e foi de transmissao (TX)
	if (EUSCI_A3->IFG & EUSCI_A_IFG_TXIFG) {
		// Buffer de transmissao recebe byte a ser transmitido
		EUSCI_A3->TXBUF = TXData;
		// Desabilita interrupcao de transmissao da SPI
		EUSCI_A3->IE &= ~EUSCI_A__TXIE;
		// Espera ate haver recepcao
		while (!(EUSCI_A3->IFG & EUSCI_A_IFG_RXIFG));
		// Salva buffer de recepcao
		RXData = EUSCI_A3->RXBUF;
		// Desabilita interrupcao de recepcao da SPI
		EUSCI_A3->IFG &= ~EUSCI_A_IFG_RXIFG;
	}
}