#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"
#include "inc/tm4c123gh6pm.h"




void timer1A_delaySec(int ttime);
void PortEPin0IntHandler(void);

int main(void)
{
    uint32_t ui32Period;

    SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);

    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
    TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);

    ui32Period = (SysCtlClockGet() / 2) / 2;
    TimerLoadSet(TIMER0_BASE, TIMER_A, ui32Period -1);

    IntEnable(INT_TIMER0A);
    TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    IntMasterEnable();

    TimerEnable(TIMER0_BASE, TIMER_A);


    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
    HWREG(GPIO_PORTE_BASE+GPIO_O_LOCK) = GPIO_LOCK_KEY;
    HWREG(GPIO_PORTE_BASE+GPIO_O_CR) |= GPIO_PIN_7;
    GPIOPinTypeGPIOInput(GPIO_PORTE_BASE,GPIO_PIN_0);
    //enables a specific event within the GPIO to generate an interrupt.
    GPIOIntEnable(GPIO_PORTF_BASE, GPIO_INT_PIN_0);
    //sets interrupt to rising edge on GPIO
    GPIOIntTypeSet(GPIO_PORTF_BASE, GPIO_INT_PIN_0, GPIO_RISING_EDGE);
    //enables the specific vector associated with GPIOF.
    IntPrioritySet(GPIO_INT_PIN_0, 0);
    IntEnable(INT_GPIOF);
    while(1)
    {

        PortEPin0IntHandler();

    }
}

void PortEPin0IntHandler(void)
{
    // Clear the GPIO interrupt
    GPIOIntClear(GPIO_PORTE_BASE, GPIO_INT_PIN_0);
    // Read the current state of the GPIO pin and
    // write back the opposite state
    if(GPIOPinRead(GPIO_PORTE_BASE, GPIO_PIN_0) != GPIO_PIN_0)
    {
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, 14);
        timer1A_delaySec(3);
    }
    else
    {
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 2);
        timer1A_delaySec(3);
    }
}

void timer1A_delaySec(int ttime)
{
    int i;

    SYSCTL_RCGCTIMER_R |= 2;
    TIMER1_CTL_R = 0;
    TIMER1_CFG_R = 0x04;
    TIMER1_TAMR_R = 0x02;
    TIMER1_TAILR_R = 64000 - 1;
    TIMER1_TAPR_R = 250 - 1;
    TIMER1_ICR_R = 0x1;
    TIMER1_CTL_R |= 0x01;
    for( i = 0; i < ttime; i++){
        while((TIMER1_RIS_R & 0x1) == 0);
        TIMER1_ICR_R = 0x1;
    }
}

void Timer0IntHandler(void)
{
    // Clear the timer interrupt
    TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

    // Read the current state of the GPIO pin and
    // write back the opposite state
    if(GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_2))
    {
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, 0);
    }
    else
    {
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 4);
    }
}
