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
	
	MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
	MAP_SysCtlDelay(3);

	MAP_IntDisable(INT_ADC0SS0);
	MAP_ADCIntDisable(ADC0_BASE, 0);
	MAP_ADCSequenceDisable(ADC0_BASE, 0);

	MAP_ADCHardwareOversampleConfigure(ADC0_BASE, 4);
	MAP_ADCSequenceConfigure(ADC0_BASE, 0, ADC_TRIGGER_PROCESSOR, 0);
	MAP_ADCSequenceStepConfigure(ADC0_BASE, 0, 0, ADC_CTL_IE | ADC_CTL_END | ADC_CTL_CH0);
	MAP_ADCSequenceEnable(ADC0_BASE, 0);

	MAP_ADCIntClear(ADC0_BASE, 0);
	MAP_ADCIntEnable(ADC0_BASE, 0);
	MAP_IntEnable(INT_ADC0SS0);
}

void ADC_handler() {
	ADCIntClear(ADC0_BASE, 0);
	ADCSequenceDataGet(ADC0_BASE, 0, &ADCValue);
}

void PWMConfigure(void) {
	SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM0);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	
	GPIOPinConfigure(GPIO_PF1_M0PWM1);
	GPIOPinTypePWM(GPIO_PORTF_BASE, GPIO_PIN_1);
	
	PWMGenConfigure(PWM0_BASE, PWM_GEN_0, PWM_GEN_MODE_DOWN | PWM_GEN_MODE_NO_SYNC);
	PWMClockSet(PWM0_BASE, PWM_SYSCLK_DIV_4);
	PWMGenPeriodSet(PWM0_BASE, PWM_GEN_0, 3000);
	PWMPulseWidthSet(PWM0_BASE, PWM_OUT_1, 1500);
	PWMGenEnable(PWM0_BASE, PWM_GEN_0);
	PWMOutputState(PWM0_BASE, (PWM_OUT_0_BIT | PWM_OUT_1_BIT), true);
	
	PWMIntEnable(PWM0_BASE, PWM_INT_GEN_0);
	IntEnable(INT_PWM0_0);
	PWMGenIntTrigEnable(PWM0_BASE, PWM_GEN_0, PWM_INT_CNT_LOAD);
}

void PWM_handler() {

	PWMValue = ADCValue*16; // 2^16/2^12

	PWMPulseWidthSet(PWM0_BASE, PWM_OUT_1, (int)(PWMValue));

	ADCIntClear(ADC0_BASE, 0);

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