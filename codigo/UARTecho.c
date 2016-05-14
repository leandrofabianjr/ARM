#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "driverlib/pin_map.h"
#include "driverlib/gpio.h"

// Altere para o tamanho do buffer de entrada desejado
#define INP_BUF_SIZ 20

void UARTConfigure() {

	// Habilita GPIO A usado na comunicacao da UART 0
	MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
	// Aguarda 3 SysCtlDelay. Aproximadamente 10 ciclos de clock
	MAP_SysCtlDelay(3);
	// Configura PA0 no modo Rx da UART 0
	MAP_GPIOPinConfigure(GPIO_PA0_U0RX);
	// Configura PA1 no modo Tx da UART 0
	MAP_GPIOPinConfigure(GPIO_PA1_U0TX);


	// Habilita UART 0
	MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
	// Configura PA0 e PA1 como pinos de comunicacao da UART
	MAP_GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
	// Configura UART 0 com fonte de clock 120MHz para 115.200 baud 8N1
	MAP_UARTConfigSetExpClk(UART0_BASE, 120000000, 115200, (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));


	// Habilita interrupcoes no processador
	MAP_IntMasterEnable();
	// Habilita interrupcao da UART 0
	MAP_IntEnable(INT_UART0);
	// Configura pinos de interrupcao da UART 0
	MAP_UARTIntEnable(UART0_BASE, UART_INT_RX | UART_INT_RT);
}

void UARTIntHandler(void) {
	uint32_t statusInterrupt;
	uint8_t buffer;

	// Salva o status de interrupcao da UART 0
	statusInterrupt = MAP_UARTIntStatus(UART0_BASE, true);

	// Limpa interrupcoes encontradas na UART 0
	MAP_UARTIntClear(UART0_BASE, statusInterrupt);


	// Enquanto houver caracteres na FIFO de transmissao para serem enviados
	while (MAP_UARTCharsAvail(UART0_BASE)) {

		// Le proximo caractere recebido
		buffer = MAP_UARTCharGetNonBlocking(UART0_BASE);
		// Envia caractere lido de volta para a UART 0
		MAP_UARTCharPutNonBlocking(UART0_BASE, buffer);
	}
}

int main(void) {

	MAP_SysCtlMOSCConfigSet(SYSCTL_MOSC_HIGHFREQ);

	MAP_SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ | SYSCTL_OSC_MAIN | SYSCTL_USE_PLL | SYSCTL_CFG_VCO_480), 120000000);


	// Funcao de inicializacao da UART
	UARTConfigure();

	while (1);

	return 0;
}


