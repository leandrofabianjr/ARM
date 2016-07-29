#include "msp432p401r.h"

// Configuracao do PWM
void PWMConfigure(void) {
	// Configura o pino P7.7 como saida do PWM
	P7->DIR |= BIT7;
	P7->SEL0 |= BIT7;

	// 1000 contagens para o peridodo do clock
	TIMER_A1->CCR[0] = 1000;
	// CCR1 em modo reset/set
	TIMER_A1->CCTL[1] = TIMER_A_CCTLN_OUTMOD_7;
	// PWM CCR1 com metade da largura de pulso
	TIMER_A1->CCR[1] = 500;
	// SMCLK = 3 MHz como fonte do PWM, modo UP, limpar contador
	TIMER_A1->CTL = TIMER_A_CTL_SSEL__SMCLK | TIMER_A_CTL_MC__UP
			| TIMER_A_CTL_CLR;
}

// Configuracao do ADC
void ADCConfigure(void) {
	// Configura pino P5.4 como entrada do ADC
	P5->SEL1 |= BIT4;
	P5->SEL0 |= BIT4;

	__enable_interrupt();
	// Habilita a interrupcao do ADC no vetor de interrupcao (NVIC)
	NVIC->ISER[0] = 1 << ((ADC14_IRQn) & 31);

	// 16 amostras do ADC, amostragem em modo pulsado, ADC ligado
	ADC14->CTL0 = ADC14_CTL0_SHT0_2 | ADC14_CTL0_SHP | ADC14_CTL0_ON;
	// Habilita ADC de 12 bits
	ADC14->CTL1 = ADC14_CTL1_RES_2;
	// Entrada do ADC como A1, tensao de referencia = VCC
	ADC14->MCTL[0] |= ADC14_MCTLN_INCH_1;
	// Habilita interrupcao do ADC
	ADC14->IER0 |= ADC14_IER0_IE0;
}

int main(void) {
	// Declaraco de contador
	volatile unsigned int i;

	// Desliga watchdog
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;

	PWMConfigure();
	ADCConfigure();

	// Acorda ao sair de uma ISR
	SCB->SCR &= ~SCB_SCR_SLEEPONEXIT_Msk;

	while (1) {
		// Delay entre leituras
		for (i = 20000; i > 0; i--);
		// Start sampling/conversion
		ADC14->CTL0 |= ADC14_CTL0_ENC | ADC14_CTL0_SC;

		// Modo de economia de energia LPM3
		__sleep();
		// Informa ao debugger que esta em modo de economia
		__no_operation();
	}
}

// ADC14 interrupt service routine
void ADC14_IRQHandler(void) {
	TIMER_A1->CCR[1] = ADC14->MEM[0] / 4.096;// CCR1 PWM duty cycle -> Valor do ADC / ((2^12)/1000)
}