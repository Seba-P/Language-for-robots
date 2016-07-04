/*! \brief The source file with pin management functions. 
*	\file pinManagement.c
*	\author \c Sebastian \c Pisklak & \c Jakub \c Olak
*	
* This file contains functions used to manipulate the state of pins.
*
* \b Copyright: Sebastian Pisklak & Jakub Olak
*/

#include "pinManagement.h"

/*! Set or clear the \c CE pin. 
* \param setClear - \c '1': set the \c CE pin, \c '0': clear the \c CE pin;
* \sa pin_CSN()
*/
void pin_CE(_Bool setClear){
	if(setClear){
		FPT_SET(PORT_CE, PIN_CE);
		CE_state=1;
	}else {
		FPT_CLEAR(PORT_CE, PIN_CE);
		CE_state=0;
	}
}

/*! Set or clear the \c CSN pin. 
* \param setClear - \c '1': set the \c CSN pin, \c '0': clear the \c CSN pin;
* \sa pin_CE()
*/
void pin_CSN(_Bool setClear){
	if(setClear){
		FPT_SET(PORT_CSN, PIN_CSN);
		CSN_state=1;
	}else {
		FPT_CLEAR(PORT_CSN, PIN_CSN);
		CSN_state=0;
	}
}

/*! Initialize the pins. 
* \detail The function initializes the pins according to *PIN SETTINGS* in \c 'pinManagement.h'.
* \sa pinManagement.h
*/
void pin_Init(void){
	SIM_SCGC5_SET(PORT_CE);
	SIM_SCGC5_SET(PORT_CSN);
	SIM_SCGC5_SET(PORT_IRQ);
	
	PORT_PCR_MUX_GPIO(PORT_CE, PIN_CE);
	PORT_PCR_MUX_GPIO(PORT_CSN, PIN_CSN);
	PORT_PCR_MUX_GPIO(PORT_IRQ, PIN_IRQ);
	
	PIN_OUTPUT(PORT_CE, PIN_CE);
	PIN_OUTPUT(PORT_CSN, PIN_CSN);
	PIN_INPUT(PORT_IRQ, PIN_IRQ);
	PIN_PULL_ENABLE(PORT_IRQ, PIN_IRQ);
	PIN_PULL_UP(PORT_IRQ, PIN_IRQ);
	
	PIN_IRQ_ENABLE(PORT_IRQ, PIN_IRQ, FALLING_EDGE);
	NVIC_ClearPendingIRQ(PIN_IRQ_IRQn);
	NVIC_EnableIRQ(PIN_IRQ_IRQn);
}

