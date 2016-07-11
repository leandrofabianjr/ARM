#include <stdbool.h>
#include <stdint.h>
#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/ssi.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "utils/uartstdio.h"

// Numero de bytes enviados e recebidos
#define NUM_SSI_DATA            3

void InitConsole(void) {
    // Habilita GPIO A
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    // Configura os pinos de recepcao e transmissao da UART 0
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);

	// Habilita a UART 0
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    // Clock da UART como o clock interno de 16 MHz
    UARTClockSourceSet(UART0_BASE, UART_CLOCK_PIOSC);
    // Habilita pinos da comunicaco UART
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    // Inicializa UART com baud rate de 115200
    UARTStdioConfig(0, 115200, 16000000);
}

int main(void) {
    uint32_t ui32SysClock;

    uint32_t pui32DataTx[NUM_SSI_DATA];
    uint32_t pui32DataRx[NUM_SSI_DATA];
    uint32_t ui32Index;

	// Configura clock do sistema como 25 MHz
    ui32SysClock = SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ |
                                       SYSCTL_OSC_MAIN |
                                       SYSCTL_USE_OSC), 25000000);

    // Inicializa o console
    InitConsole();

    // Mostra a situcao do sistema na=o terminal UART
    UARTprintf("SSI ->\n");
    UARTprintf("  Mode: SPI\n");
    UARTprintf("  Data: 8-bit\n\n");

    // Habilita SSI
    SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI0);

    // Habilita a GPIO A
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    // Configura os pinos da comunicacao SPI
    //      PA4 - SSI0Rx
    //      PA3 - SSI0Fss
    //      PA2 - SSI0CLK
    GPIOPinConfigure(GPIO_PA2_SSI0CLK);
    GPIOPinConfigure(GPIO_PA3_SSI0FSS);
    GPIOPinConfigure(GPIO_PA4_SSI0RX);
    GPIOPinConfigure(GPIO_PA5_SSI0TX);

    // Configura pinos utilisados na SSI
    GPIOPinTypeSSI(GPIO_PORTA_BASE,
		GPIO_PIN_5 | GPIO_PIN_4 | GPIO_PIN_3 | GPIO_PIN_2);

    // Configuracao da comunicacao
    SSIConfigSetExpClk(SSI0_BASE, ui32SysClock, SSI_FRF_MOTO_MODE_0,
                       SSI_MODE_MASTER, 1000000, 8);

    // Habilita SSI
    SSIEnable(SSI0_BASE);

    // Aguarda leitura de todos os dados recebidos
    while(SSIDataGetNonBlocking(SSI0_BASE, &pui32DataRx[0]))
    {
    }

    // Texto a ser enviado
    pui32DataTx[0] = 's';
    pui32DataTx[1] = 'p';
    pui32DataTx[2] = 'i';

    //  Indicacao de que comecara o envio
    UARTprintf("Sent:\n  ");

    // Envia 3 bytes de dado
    for(ui32Index = 0; ui32Index < NUM_SSI_DATA; ui32Index++)
    {
        // Mostra na UART, o dado que esta sendo transferido
        UARTprintf("'%c' ", pui32DataTx[ui32Index]);

        // Envia byte dela SSI
        SSIDataPut(SSI0_BASE, pui32DataTx[ui32Index]);
    }

    // Aguarda SSI estar pronta
    while(SSIBusy(SSI0_BASE))
    {
    }

    // Indicacao de que SSI esta recebendo dados
    UARTprintf("\nReceived:\n  ");

    // Recebe 3 bytes de dado
    for(ui32Index = 0; ui32Index < NUM_SSI_DATA; ui32Index++)
    {
        // Recebe 3 bytes de dado pela SSI
        SSIDataGet(SSI0_BASE, &pui32DataRx[ui32Index]);

        // Conversao para 8 bits
        pui32DataRx[ui32Index] &= 0x00FF;

        // Mostra byte de dado recebido
        UARTprintf("'%c' ", pui32DataRx[ui32Index]);
    }

    // Retorna sem erro
    return(0);
}
