#include "msp432p401r.h"

int main(void) {
	// Desliga watchdog timer
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;

	// Configura pino P1.0 (LED) como saida
	P1->DIR |= BIT0;
	P1->OUT |= BIT0;

	// Habilita interrupcao do Timer A0
	TIMER_A0->CCTL[0] = TIMER_A_CCTLN_CCIE;
	// Estouro de contagem em do timer em 5000
	TIMER_A0->CCR[0] = 50000;
	// Fonte de clock como SMCLK, modo continuo
	TIMER_A0->CTL = TIMER_A_CTL_SSEL__SMCLK | TIMER_A_CTL_MC__CONTINUOUS;

	// Habilita modo de economia ao sair da ISR
	SCB->SCR |= SCB_SCR_SLEEPONEXIT_Msk;

	// Habilita as interrupcoes no processador
	__enable_interrupt();
	// Habilita interrupcao do Timer A0
	NVIC->ISER[0] = 1 << ((TA0_0_IRQn) & 31);

	// Superlaco
	while (1) {
		// Modo de economia de energia LPM3
		__sleep();
		// Informa ao debugger que esta em modo de economia
		__no_operation();
	}
}

// Rotina de tratamento de interrupcao do Timer A0
void TA0_0_IRQHandler(void) {
	// Desabilita interrupcao do Timer A0
	TIMER_A0->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG;
	// Inverte estado do LED
	P1->OUT ^= BIT0;
	// Soma 5000 ao contador do Timer A0
	TIMER_A0->CCR[0] += 50000;
}