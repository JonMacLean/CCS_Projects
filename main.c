
///*********************************************************************
//|                         1._Documentation_
//|
//|  FILE:            Main.c
//|
//|  DEV INFO:        _____________________________________________________
//|                  |Author    : Jon MacLean
//|                  |Position  : Software Engineer
//|                  |Company   : Kongsberg Maritime
//|                  |Email     : jdmaclean6@gmail.com
//|                  |Date      : 28.05.2014
//|
//|
//|  REFERENCE:       ________________________________________________________________________________
//|  	   	         |Website       :
//|  	 	         |DataSheet     :
//|  		         |Manufactuerer :
//|                  |Device        : SMA42056 7 Segment Display with Decimal Point
//|
//|  APPLICATION:     ___________________________________________________________
//|                  |
//|                  |
//|
//|
//|  HW CONNECTIONS:  ___________________________________________________________
//|                  |
//|                  |
//|
//|           10 9 8 7 6
//|            |_|_|_|_|
//|
//|               A=7
//|            +-------+
//|            |       |
//|        F=9 | G=10  | B=6
//|            +-------+
//|            |       |
//|        E=1 |  D=2  | C=4
//|            +-------+ [] DP=5
//|             _ _ _ _
//|            | | | | |
//|            1 2 3 4 5
//|
//|			 HW Connections
//|                               // BASE B
//| 		LED_A_TOPMID        7	   // GPIO_PIN_0
//| 		LED_B_RIGHTTOP      6 	   // GPIO_PIN_1
//| 		LED_C_RIGHTBOT      4	   // GPIO_PIN_2
//| 		LED_D_BOTMID        2	   // GPIO_PIN_3
//| 		LED_E_LEFTBOT       1	   // GPIO_PIN_4
//| 		LED_F_LEFTTOP       9	   // GPIO_PIN_5
//| 		LED_G_MIDMID        10	   // GPIO_PIN_6
//| 		LED_DP_DECPOINT     5	   // GPIO_PIN_7
//| 		LED_GND             3,8     // GND           No SW is required for LED_GND, just a cable connection from 3 and 8
//|
//|  Copywrite (C) - 28.05.2014
//|
///*********************************************************************





///*********************************************************************
//|                      2._Preprocessor Directives_
//|
//|
///*********************************************************************
#include <stdint.h>                    // These must go before local directives that use the variables.
#include <stdbool.h>
#include "inc/tm4c123gh6pm.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "inc/hw_gpio.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/timer.h"



///*********************************************************************
//|                             3._Prototypes_
//|
//|
///*********************************************************************



///*********************************************************************
//|                             4._DEFINITIONS_
//|
//|  BACKGROUND: _______________________________________________________
//|                The definitions map the [Pin] number to the labeling
//|              provided in the datasheet for PCD8544 48 × 84 pixels
//|              matrix LCD controller/driver.
//|
//|
//|  CONVENTION: ________________________________________________________
//|              [PinLabel][Pin]
//|
///*********************************************************************



///*********************************************************************
//|                          5._Global Variables_
//|
//|
///*********************************************************************





///*********************************************************************
//|                          6._Main Routine_
//|
//|
///*********************************************************************


int main(void){

	// Clock Config 40 Mhz default /2 divider for the 400 Mhz pll
	SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);

	// Enable Port
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);

	GPIODirModeSet(GPIO_PORTB_BASE, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7,  GPIO_DIR_MODE_OUT);
	GPIOPadConfigSet(GPIO_PORTB_BASE, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7, GPIO_STRENGTH_8MA, GPIO_PIN_TYPE_STD);

	uint32_t ui32Period;

	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
	TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);

	ui32Period = (SysCtlClockGet() / 10) / 2; // 2 Mhz
	TimerLoadSet(TIMER0_BASE, TIMER_A, ui32Period -1);

	IntEnable(INT_TIMER0A);
	TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
	IntMasterEnable();

	TimerEnable(TIMER0_BASE, TIMER_A);

	//SysCtlDelay(2000000);

}





void Timer0IntHandler(void)
{
	// Clear the timer interrupt
	TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

	// Read the current state of the GPIO pin and
	// write back the opposite state
	if(GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_2))
		GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7, 0xFF);

}

#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_ssi.h"
#include "inc/hw_types.h"
#include "driverlib/ssi.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"

#define NUM_SSI_DATA 8
const uint8_t pui8DataTx[NUM_SSI_DATA] =
{0x88, 0xF8, 0xF8, 0x88, 0x01, 0x1F, 0x1F, 0x01};

// Bit-wise reverses a number.
uint8_t
Reverse(uint8_t ui8Number)
{
	uint8_t ui8Index;
	uint8_t ui8ReversedNumber = 0;
	for(ui8Index=0; ui8Index<8; ui8Index++)
	{
		ui8ReversedNumber = ui8ReversedNumber << 1;
		ui8ReversedNumber |= ((1 << ui8Index) & ui8Number) >> ui8Index;
	}
	return ui8ReversedNumber;
}

int main(void)
{
	uint32_t ui32Index;
	uint32_t ui32Data;

	SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);

	SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI0);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

	GPIOPinConfigure(GPIO_PA2_SSI0CLK);
	GPIOPinConfigure(GPIO_PA3_SSI0FSS);
	GPIOPinConfigure(GPIO_PA5_SSI0TX);
	GPIOPinTypeSSI(GPIO_PORTA_BASE,GPIO_PIN_5|GPIO_PIN_3|GPIO_PIN_2);

	SSIConfigSetExpClk(SSI0_BASE, SysCtlClockGet(), SSI_FRF_MOTO_MODE_0, SSI_MODE_MASTER, 10000, 16);
	SSIEnable(SSI0_BASE);

	while(1)
	{
		for(ui32Index = 0; ui32Index < NUM_SSI_DATA; ui32Index++)
		{
			ui32Data = (Reverse(pui8DataTx[ui32Index]) << 8) + (1 << ui32Index);
			SSIDataPut(SSI0_BASE, ui32Data);
			while(SSIBusy(SSI0_BASE))
			{
			}
		}

	}

}
