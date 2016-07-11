#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "driverlib/pin_map.h"
#include "driverlib/gpio.h"

// Numero maximo de perifericos existentes
#define peripheralQuantity	77

// Definicoes dos perifericos que podem estar disponiveis no sistema
const uint32_t peripheralAvailableId[peripheralQuantity] = {
	SYSCTL_PERIPH_ADC0, SYSCTL_PERIPH_ADC1, SYSCTL_PERIPH_CAN0,
	SYSCTL_PERIPH_CAN1, SYSCTL_PERIPH_COMP0, SYSCTL_PERIPH_EMAC0,
	SYSCTL_PERIPH_EPHY0, SYSCTL_PERIPH_EPI0, SYSCTL_PERIPH_GPIOA,
	SYSCTL_PERIPH_GPIOB, SYSCTL_PERIPH_GPIOC, SYSCTL_PERIPH_GPIOD,
	SYSCTL_PERIPH_GPIOE, SYSCTL_PERIPH_GPIOF, SYSCTL_PERIPH_GPIOG,
	SYSCTL_PERIPH_GPIOH, SYSCTL_PERIPH_GPIOJ, SYSCTL_PERIPH_HIBERNATE,
	SYSCTL_PERIPH_CCM0, SYSCTL_PERIPH_EEPROM0, SYSCTL_PERIPH_FAN0,
	SYSCTL_PERIPH_FAN1, SYSCTL_PERIPH_GPIOK, SYSCTL_PERIPH_GPIOL,
	SYSCTL_PERIPH_GPIOM, SYSCTL_PERIPH_GPION, SYSCTL_PERIPH_GPIOP,
	SYSCTL_PERIPH_GPIOQ, SYSCTL_PERIPH_GPIOR, SYSCTL_PERIPH_GPIOS,
	SYSCTL_PERIPH_GPIOT, SYSCTL_PERIPH_I2C0, SYSCTL_PERIPH_I2C1,
	SYSCTL_PERIPH_I2C2, SYSCTL_PERIPH_I2C3, SYSCTL_PERIPH_I2C4,
	SYSCTL_PERIPH_I2C5, SYSCTL_PERIPH_I2C6, SYSCTL_PERIPH_I2C7,
	SYSCTL_PERIPH_I2C8, SYSCTL_PERIPH_I2C9, SYSCTL_PERIPH_LCD0,
	SYSCTL_PERIPH_ONEWIRE0, SYSCTL_PERIPH_PWM0, SYSCTL_PERIPH_PWM1,
	SYSCTL_PERIPH_QEI0, SYSCTL_PERIPH_QEI1, SYSCTL_PERIPH_SSI0,
	SYSCTL_PERIPH_SSI1, SYSCTL_PERIPH_SSI2, SYSCTL_PERIPH_SSI3,
	SYSCTL_PERIPH_TIMER0, SYSCTL_PERIPH_TIMER1, SYSCTL_PERIPH_TIMER2,
	SYSCTL_PERIPH_TIMER3, SYSCTL_PERIPH_TIMER4, SYSCTL_PERIPH_TIMER5,
	SYSCTL_PERIPH_TIMER6, SYSCTL_PERIPH_TIMER7, SYSCTL_PERIPH_UART0,
	SYSCTL_PERIPH_UART1, SYSCTL_PERIPH_UART2, SYSCTL_PERIPH_UART3,
	SYSCTL_PERIPH_UART4, SYSCTL_PERIPH_UART5, SYSCTL_PERIPH_UART6,
	SYSCTL_PERIPH_UART7, SYSCTL_PERIPH_UDMA, SYSCTL_PERIPH_USB0,
	SYSCTL_PERIPH_WDOG0, SYSCTL_PERIPH_WDOG1, SYSCTL_PERIPH_WTIMER0,
	SYSCTL_PERIPH_WTIMER1, SYSCTL_PERIPH_WTIMER2, SYSCTL_PERIPH_WTIMER3,
	SYSCTL_PERIPH_WTIMER4, SYSCTL_PERIPH_WTIMER5
};

// Nomes dos perifericos que podem estar disponiveis no sistema
const uint8_t peripheralDescription[peripheralQuantity][23] = {
	"Conversor A/D 0", "Conversor A/D 1", "Barramento CAN 0 ",
	"Barramento CAN 1 ", "Comparador analogico 0", "Ethernet MAC 0",
	"Ethernet PHY 0", "EPI0", "GPIO A",
	"GPIO B", "GPIO C", "GPIO D",
	"GPIO E", "GPIO F", "GPIO G",
	"GPIO H", "GPIO J", "Modulo de hibernacao",
	"CCM 0", "EEPROM 0", "FAN 0",
	"FAN 1", "GPIO K", "GPIO L",
	"GPIO M", "GPIO N", "GPIO P",
	"GPIO Q", "GPIO R", "GPIO S",
	"GPIO T", "I2C 0", "I2C 1",
	"I2C 2", "I2C 3", "I2C 4",
	"I2C 5", "I2C 6", "I2C 7",
	"I2C 8", "I2C 9", "LCD 0",
	"One Wire 0", "PWM 0", "PWM 1",
	"QEI 0", "QEI 1", "SSI 0",
	"SSI 1", "SSI 2", "SSI 3",
	"Timer 0", "Timer 1", "Timer 2",
	"Timer 3", "Timer 4", "Timer 5",
	"Timer 6", "Timer 7", "UART 0",
	"UART 1", "UART 2", "UART 3",
	"UART 4", "UART 5", "UART 6",
	"UART 7", "uDMA", "USB 0",
	"Watchdog 0", "Watchdog 1", "Wide Timer 0",
	"Wide Timer 1", "Wide Timer 2", "Wide Timer 3",
	"Wide Timer 4", "Wide Timer 5"
};

// Imprime uma string na UART
void UARTprint(uint8_t *buffer) {
	int i;
	for(i = 0; i < strlen((char*)buffer); i++) {
		UARTCharPut(UART0_BASE, buffer[i]);
	}
}

// Imprime na UART os perifericos disponiveis no sistema
void UARTPrintPeripheralsAvailable() {
	int i;
	UARTprint((uint8_t*)"Perifericos disponiveis:\n\r");
	for (i = 0; i < peripheralQuantity; i++) {
		if (SysCtlPeripheralPresent(
				(uint32_t)peripheralAvailableId[i])) {
			UARTprint((uint8_t*)"- ");
			UARTprint((uint8_t*)peripheralDescription[i]);
			UARTprint((uint8_t*)"\n\r");
		}
	}
}

// Rotina de tratamento de interrupcao da UART
void UARTIntHandler(void) {
	uint32_t statusInterrupt;

	// Salva o status de interrupcao da UART 0
	statusInterrupt = MAP_UARTIntStatus(UART0_BASE, true);

	// Limpa interrupcoes encontradas na UART 0
	MAP_UARTIntClear(UART0_BASE, statusInterrupt);

	// Enquanto houver caracteres na FIFO de transmissao
	// para serem enviados
	while (MAP_UARTCharsAvail(UART0_BASE)) {

		// Se o caractere informado por 'p' entao sao
		// listados os perifericos disponiveis no sistema
		if (UARTCharGet(UART0_BASE) == 'p') {
			UARTPrintPeripheralsAvailable();
		} else {
			UARTprint((uint8_t*)"Para listar os "
					"perifericos disponiveis envie a letra 'p'\n\r");
		}
	}
}

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
	UARTConfigSetExpClk(UART0_BASE, 120000000, 115200,
			(UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | 
			UART_CONFIG_PAR_NONE));


	// Configura rotina de tratamento de interrupcao da UART
	UARTIntRegister(UART0_BASE, UARTIntHandler);
	// Habilita interrupcao da UART 0
	MAP_IntEnable(INT_UART0);
	// Configura pinos de interrupcao da UART 0
	MAP_UARTIntEnable(UART0_BASE, UART_INT_RX | UART_INT_RT);
}1

// Funcao principal do programa
int main(void) {

	// Configura oscilador principal acima de 10 MHz
	SysCtlMOSCConfigSet(SYSCTL_MOSC_HIGHFREQ);

	// Configura clock para 120 MHz
	MAP_SysCtlClockFreqSet(
			(SYSCTL_XTAL_25MHZ | SYSCTL_OSC_MAIN | SYSCTL_USE_PLL |
			SYSCTL_CFG_VCO_480),
			120000000);

	// Funcao de inicializacao da UART
	UARTConfigure();

	// Super laco de programa
	while (1);

	return 0;
}
