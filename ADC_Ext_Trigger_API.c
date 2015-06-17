// #define PART_TM4C123GH6PM

#include <stdint.h>
#include <stdbool.h>
#include "stdlib.h"
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_uart.h"
#include "inc/hw_gpio.h"
#include "inc/hw_pwm.h"
#include "inc/hw_types.h"
#include "driverlib/adc.h"
#include "driverlib/timer.h"
#include "driverlib/gpio.c"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.c"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "driverlib/udma.h"
#include "driverlib/pwm.h"
#include "driverlib/ssi.h"
#include "driverlib/systick.h"
#include "driverlib/adc.h"
#include "utils/uartstdio.h"
#include "utils/uartstdio.c"
#include <string.h>

void InitPortEF(void);      // Function to initialise Port E and Port F

//volatile uint32_t value=0;  // global variable - Stores the value from PF4
//*****************************************************************************
//
// This function sets up UART0 to be used for a console to display information
// as the example is running.
//
//*****************************************************************************
void
InitConsole(void)
{
    //
    // Enable GPIO port A which is used for UART0 pins.
    // TODO: change this to whichever GPIO port you are using.
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    //
    // Configure the pin muxing for UART0 functions on port A0 and A1.
    // This step is not necessary if your part does not support pin muxing.
    // TODO: change this to select the port/pin you are using.
    //
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);

    //
    // Enable UART0 so that we can configure the clock.
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);

    //
    // Use the internal 16MHz oscillator as the UART clock source.
    //
    UARTClockSourceSet(UART0_BASE, UART_CLOCK_PIOSC);

    //
    // Select the alternate (UART) function for these pins.
    // TODO: change this to select the port/pin you are using.
    //
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    //
    // Initialize the UART for console I/O.
    //
    UARTStdioConfig(0, 115200, 16000000);
}

void InitPortEF(void) {
	
	// Enable Port E
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
	// Enable Port F
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	// Delay to let the clock stabilise
	SysCtlDelay(3);
	
	// For making PE2 as ADC input pin
	GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_2);
	
	/*
    Configure the switch on the left of the launchpad, GPIO_PIN_4 to a input with
    internal pull-up.
  */
  GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, GPIO_PIN_4);
  GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_4, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
	GPIOIntTypeSet(GPIO_PORTF_BASE,GPIO_PIN_4,GPIO_FALLING_EDGE);
	// Make PF4 a trigger for ADC
	GPIOADCTriggerEnable(GPIO_PORTF_BASE, GPIO_PIN_4);
}

int main(){
	  SysCtlClockSet(SYSCTL_SYSDIV_2_5|SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ);
	  InitConsole();
		InitPortEF();
	    //
	    // This array is used for storing the data read from the ADC FIFO. It
	    // must be as large as the FIFO for the sequencer in use.  This example
	    // uses sequence 3 which has a FIFO depth of 1.  If another sequence
	    // was used with a deeper FIFO, then the array size must be changed.
	    //
	    uint32_t ADCValues[1];
			uint32_t value=0;  // Stores the value from PF4
	    
	    //
	    // Display the setup on the console.
	    //
	    UARTprintf("ADC ->\n");
	    UARTprintf("  Type: Slide Potentiometer\n");
	    UARTprintf("  Samples: One\n");
	    UARTprintf("  Updates on PF4 press");
	    UARTprintf("  Input Pin: PE2\n\n");

	    //
	    // The ADC0 peripheral must be enabled for use.
	    //
	    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
	    SysCtlDelay(3);

	    //
	    // Enable sample sequence 3 with a external signal trigger.  Sequence 3
	    // will do a single sample when the processor sends a singal to start the
	    // conversion.  Each ADC module has 4 programmable sequences, sequence 0
	    // to sequence 3.  This example is arbitrarily using sequence 3.
	    //
	    ADCSequenceConfigure(ADC0_BASE, 3, ADC_TRIGGER_EXTERNAL, 0);

	    //
	    // Configure step 0 on sequence 3.  Sample channel 1 (ADC_CTL_CH1) and configure the interrupt flag (ADC_CTL_IE) to be set
	    // when the sample is done.  Tell the ADC logic that this is the last
	    // conversion on sequence 3 (ADC_CTL_END).  Sequence 3 has only one
	    // programmable step.  Sequence 1 and 2 have 4 steps, and sequence 0 has
	    // 8 programmable steps.  Since we are only doing a single conversion using
	    // sequence 3 we will only configure step 0.  For more information on the
	    // ADC sequences and steps, reference the datasheet.
	    //
	    ADCSequenceStepConfigure(ADC0_BASE, 3, 0, ADC_CTL_CH1 | ADC_CTL_IE |
	                             ADC_CTL_END);

	    //
	    // Since sample sequence 3 is now configured, it must be enabled.
	    //
	    ADCSequenceEnable(ADC0_BASE, 3);

	    //
	    // Clear the interrupt status flag.  This is done to make sure the
	    // interrupt flag is cleared before we sample.
	    //
	    ADCIntClear(ADC0_BASE, 3);

	    //
	    // Sample the ADC forever.  Display the value on the
	    // console.
	    //
	    while(1)
	    {
				  SysCtlDelay(7000000);
	        //
	        // Trigger the ADC conversion.
	        //
	        // ADCProcessorTrigger(ADC0_BASE, 3);
					
					// Read the PF4 state into value
	        value = GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_4);
				
					// If PF4 is pressed, trigger ADC conversion
				  if(( value & GPIO_PIN_4 )==0) {
							//
							// Wait for conversion to be completed.
							//
							while(!ADCIntStatus(ADC0_BASE, 3, false))
							{
							}

							//
							// Clear the ADC interrupt flag.
							//
							ADCIntClear(ADC0_BASE, 3);

							//
							// Read ADC Value.
							//
							ADCSequenceDataGet(ADC0_BASE, 3, ADCValues);
												
							//
							// Display the ADCValue on the console.
							//
							UARTprintf("ADC VALUE: %4d\r", ADCValues[0]);
							
							// A rude way to avoid bouncing
							SysCtlDelay(7000000);
						
					}
	        //
	        // This function provides a means of generating a constant length
	        // delay.  The function delay (in cycles) = 3 * parameter.  Delay
	        // 250ms arbitrarily.
	        //
	        //SysCtlDelay(80000000 / 12);
	    }
}
