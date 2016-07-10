#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_gpio.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/pin_map.h"
#include "driverlib/gpio.h"
#include "driverlib/adc.h"
#include "driverlib/pwm.h"

uint32_t ADCValue;
float PWMValue = 0;

void ADCConfigure() {
	
	// Habilita ADC0
	MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
	// Aguarda 3 SysCtlDelay. Aproximadamente 10 ciclos de clock
	MAP_SysCtlDelay(3);

	// Desabilitar Interrupcao do ADC para configura-la
	MAP_IntDisable(INT_ADC0SS0);
	MAP_ADCIntDisable(ADC0_BASE, 0);
	MAP_ADCSequenceDisable(ADC0_BASE, 0);
	
	//Configurando ADC
	MAP_ADCHardwareOversampleConfigure(ADC0_BASE, 4);
	MAP_ADCSequenceConfigure(ADC0_BASE, 0, ADC_TRIGGER_PROCESSOR, 0);
	MAP_ADCSequenceStepConfigure(ADC0_BASE, 0, 0, ADC_CTL_IE | ADC_CTL_END | ADC_CTL_CH0);
	MAP_ADCSequenceEnable(ADC0_BASE, 0);

	// Habilitando Interrupcao do ADC
	MAP_ADCIntClear(ADC0_BASE, 0);
	MAP_ADCIntEnable(ADC0_BASE, 0);
	MAP_IntEnable(INT_ADC0SS0);
}

void ADC_handler() {
	
	//Limpando Interrupcao do ADC
	ADCIntClear(ADC0_BASE, 0);
	//Passando valor convertido pelo ADC para a variavel ADCValue
	ADCSequenceDataGet(ADC0_BASE, 0, &ADCValue);
}

void PWMConfigure(void) {

	//Configurando GPIO para PWM
	SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM0);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	
	GPIOPinConfigure(GPIO_PF1_M0PWM1);
	GPIOPinTypePWM(GPIO_PORTF_BASE, GPIO_PIN_1);
	
	//Configurando PWM 
	PWMGenConfigure(PWM0_BASE, PWM_GEN_0, PWM_GEN_MODE_DOWN | PWM_GEN_MODE_NO_SYNC);
	//Configurando Fonte Clock do PWM como fonte de 120 MHz dividido por 4 sendo um clock de 30 MHz 
	PWMClockSet(PWM0_BASE, PWM_SYSCLK_DIV_4);
	//Configurando contagem de 3000, para gerar um PWM com 10 KHz
	PWMGenPeriodSet(PWM0_BASE, PWM_GEN_0, 3000);
	//Configurando contagem de 1550, para gerar um Duty Cicle inicial de 50%
	PWMPulseWidthSet(PWM0_BASE, PWM_OUT_1, 1500);
	
	//Habilitando geracao PWM
	PWMGenEnable(PWM0_BASE, PWM_GEN_0);
	PWMOutputState(PWM0_BASE, (PWM_OUT_0_BIT | PWM_OUT_1_BIT), true);
	
	//Habilitando interrupcao do PWM
	PWMIntEnable(PWM0_BASE, PWM_INT_GEN_0);
	IntEnable(INT_PWM0_0);
	PWMGenIntTrigEnable(PWM0_BASE, PWM_GEN_0, PWM_INT_CNT_LOAD);
}

void PWM_handler() {
	
	//Convertendo valor obtido pelo AD, na base 2^12, para base de 3000
	PWMValue = ADCValue / 1.3653334; // ADCValue/(2^12)*3000

	//Atualizando Duty Cicle
	PWMPulseWidthSet(PWM0_BASE, PWM_OUT_1, (int)(PWMValue));

	//Limpando Interrupcao do ADC
	ADCIntClear(ADC0_BASE, 0);
	
	//Habilitando Interrupao do ADC
	ADCProcessorTrigger(ADC0_BASE, 0);
}

int main(void) {
	
	//Configuracao Basica do Sistema de clock, selecionando a frequencia de 120 MHz
	SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ | SYSCTL_OSC_MAIN | SYSCTL_USE_PLL | SYSCTL_CFG_VCO_480), 120000000);

	//Habilitando Unidade de Ponto Flutuante
	FPUEnable(); 
	FPULazyStackingEnable();

	//Configuracao ADC
	ADCConfigure();
	
	//Configuracao PWM
	PWMConfigure();

	while (1) {

	}

	return 0;
}