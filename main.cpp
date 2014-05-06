extern "C" {
#include <math.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_i2c.h"
#include "inc/hw_ints.h"
#include "driverlib/interrupt.h"
#include "driverlib/i2c.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "driverlib/gpio.h"
#include "driverlib/uart.h"
#include "utils/uartstdio.h"
#include "utils/ustdlib.h"
#include <stdint.h>

#define SYSTICKS_PER_SECOND     1000

void SysTickHandler();

uint32_t millis();


void InitConsole(void)
{
	//
	// Enable GPIO port A which is used for UART0 pins.
	// TODO: change this to whichever GPIO port you are using.
	//
	MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

	//
	// Select the alternate (UART) function for these pins.
	// TODO: change this to select the port/pin you are using.
	//
	MAP_GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

	//
	// Initialize the UART for console I/O.
	//
	UARTStdioInit(0);
}


}

//#include "rf24/RF24.h"
#include "RF24_network/RF24Network.h"
static unsigned long ulClockMS=0;

// Address of our node
const uint16_t this_node = 0;

// Address of the other node
const uint16_t other_node = 1;

// Structure of our payload
struct payload_t
{
  unsigned long ms;
  unsigned long counter;
};


int main(void)
{
	MAP_SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN |SYSCTL_XTAL_12MHZ); //50MHZ

	//
	// Enable peripherals to operate when CPU is in sleep.
	//
	MAP_SysCtlPeripheralClockGating(true);

	//
	// Configure SysTick to occur 1000 times per second, to use as a time
	// reference.  Enable SysTick to generate interrupts.
	//
	MAP_SysTickPeriodSet(MAP_SysCtlClockGet() / SYSTICKS_PER_SECOND);
	MAP_SysTickIntEnable();
	MAP_SysTickEnable();

	//
	// Get the current processor clock frequency.
	//
	ulClockMS = MAP_SysCtlClockGet() / (3 * 1000);

	InitConsole();

	RF24 radio = RF24();
	RF24Network network = RF24Network(radio);

	radio.begin();
	network.begin(/*channel*/ 90, /*node address*/ this_node);


	UARTprintf("ok...");
	UARTprintf("RF24Network/examples/helloworld_rx/");

	while (1) {


		  // Pump the network regularly
		  network.update();

		  // If so, grab it and print it out
		      RF24NetworkHeader header;
		      payload_t payload;
		      network.read(header,&payload,sizeof(payload));
		      UARTprintf("Received packet #");
		      UARTprintf("%d",payload.counter);
		      UARTprintf(" at ");
		      UARTprintf("%d",payload.ms);

		      MAP_SysCtlDelay(ulClockMS*1000);
	}

}


static unsigned long milliSec = 0;

#ifdef __cplusplus
extern "C"
{
#endif
void SysTickHandler()
{
	milliSec++;
}

uint32_t millis(){
	return milliSec;
}
#ifdef __cplusplus
}
#endif


