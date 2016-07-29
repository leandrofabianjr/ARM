#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/debug.h"
#include "driverlib/fpu.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"
#include "driverlib/uart.h"
#include "utils/uartstdio.h"

// Armazena o clock do sistema
uint32_t g_ui32SysClock;

// Flag que controla o LED
uint32_t g_ui32Flags;

void
Timer0IntHandler(void)
{

    // Limpa a interrupcao do temporizador
    ROM_TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

    // Troca valor da flag entre 0 e 1
    HWREGBITW(&g_ui32Flags, 0) ^= 1;

    // Acende o LED 0 conforme a flag
    GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0, g_ui32Flags);

    // Atualiza a interrupcao
    ROM_IntMasterDisable();
    ROM_IntMasterEnable();
}

void
Timer1IntHandler(void)
{

	// Limpa a interrupcao do temporizador
    ROM_TimerIntClear(TIMER1_BASE, TIMER_TIMA_TIMEOUT);

    // Troca valor da flag entre 0 e 1
    HWREGBITW(&g_ui32Flags, 1) ^= 1;

    // Acende o LED 1 conforme a flag
    GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_1, g_ui32Flags);

    // Atualiza a interrupcao
    ROM_IntMasterDisable();
    ROM_IntMasterEnable();
}

int
main(void)
{
    // Congfigura clock do sistema em 120 MHz
    g_ui32SysClock = MAP_SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ |
                                             SYSCTL_OSC_MAIN |
                                             SYSCTL_USE_PLL |
                                             SYSCTL_CFG_VCO_480), 120000000);

    // Habilita as GPIOS que contem os LEDs
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);

    // Habilita os pinos que contem os LEDs
    ROM_GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_0 | GPIO_PIN_1);


    // Habilita os temporizadores 0 e 1
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1);

    // Habilita as interrupcoes no processador
    ROM_IntMasterEnable();

    // Configura os 2 temporizadores periodicos de 32 bits
    ROM_TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);
    ROM_TimerConfigure(TIMER1_BASE, TIMER_CFG_PERIODIC);
    ROM_TimerLoadSet(TIMER0_BASE, TIMER_A, g_ui32SysClock);
    ROM_TimerLoadSet(TIMER1_BASE, TIMER_A, g_ui32SysClock / 2);

    // Configura as rotinas de tratamento de interrupcao do temporizadores
    TimerIntRegister(TIMER0_BASE, TIMER_A, Timer0IntHandler);
    TimerIntRegister(TIMER1_BASE, TIMER_A, Timer1IntHandler);

    // Configura as interrupcoes dos estouros de tempo dos temporizadores
    ROM_IntEnable(INT_TIMER0A);
    ROM_IntEnable(INT_TIMER1A);
    ROM_TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    ROM_TimerIntEnable(TIMER1_BASE, TIMER_TIMA_TIMEOUT);

    // Habilita os temporizadores
    ROM_TimerEnable(TIMER0_BASE, TIMER_A);
    ROM_TimerEnable(TIMER1_BASE, TIMER_A);

    // Superlaco de programa
    while(1);

    // Retorna sem erro
    return 0;
}
