/*! \brief The header file with pin management functions, macros and defines. 
*	\file pinManagement.h
*	\author \c Sebastian \c Pisklak & \c Jakub \c Olak
*	
* This file contains useful functions, macros and defines ready for use.
* \par Just modify the pins locations in *PINS DEFINES* section and optionally a \c 'pin_Init()' function and IRQ handler.
* \note In order to use \c IRQ pin to handle interrupts coming from \b nRF24L01+ module, you have to locate the \c IRQ pin on PORTA, PORTC or PORTD. The \c IRQ pin initialization in \c 'pin_Init()' function must have pull up resistor enabled and an IRQ has to be trigerred on falling edge.
*
* \b Copyright: Sebastian Pisklak & Jakub Olak
*/

#ifndef PINMANAGEMENT_H
	#define PINMANAGEMENT_H
	
	#include "MKL46Z4.h"
	
	/*! \name VALUE MNEMONICS
	* Useful value mnemonics.
	* @{
	*/
	/*****************
	* VALUE MNEMONICS
	*****************/
	/*! Definition of high state for pin. */
	#define HIGH 	1
	/*! Definition of low state for pin. */
	#define LOW 	0
	
	/*! Low level IRQ trigger. */
	#define LOW_LEVEL 		0x08
	/*! Rising edge IRQ trigger. */
	#define RISING_EDGE 	0x09
	/*! Falling edge IRQ trigger. */
	#define FALLING_EDGE 	0x0A
	/*! Both edges IRQ trigger. */
	#define BOTH_EDGES 		0x0B
	/*! High level IRQ trigger. */
	#define HIGH_LEVEL 		0x0C
	//!@}
	
	/*! \name PINS DEFINES
	* Defines for pins location. Modify them according to your needs.
	* \note Make sure the pins location does not interfere with other modules I/O.
	* \par It is recommended to locate all three pins on the same port. The PORTA,PORTC or PORTD is recommended due to IRQ handler available for the IRQ pin.
	* @{
	*/
	/**************
	* PINS DEFINES
	**************/
	/*! The port for \c IRQ pin.
	* \warning The \c IRQ pin must be located in PORTA, PORTC or PORTD due to lack of IRQ handlers for other ports.
	*/
	#define PORT_IRQ			D
	/*! The number of pin for \c IRQ pin. */
	#define PIN_IRQ				2
	/*! The \c IRQn enumerator for \c IRQ pin.
	* \note Available values: \c PORTA_IRQn, \c PORTC_PORTD_IRQn.
	*/
	#define PIN_IRQ_IRQn	PORTC_PORTD_IRQn
	
	/*! The port for \c CE pin. */
	#define PORT_CE				D
	/*! The number of pin for \c CE pin. */
	#define PIN_CE				4
	
	/*! The port for \c CSN pin. */
	#define PORT_CSN			D
	/*! The number of pin for \c CSN pin. */
	#define PIN_CSN				6
	//!@}
	
	/*! \name MACROOPERATIONS
	* Universal set of macrooperations used to manipulate the pin controls and registers. No modification needed, just edit pins location constants and use this set in your code.
	* @{
	*/
	/*****************
	* MACROOPERATIONS
	*****************/
	/*! Simple macro for two strings concatenation.
	* \detail GLUE(what,x) => whatx
	*/
	#define GLUE(what, x) 										(what##x)
	/*! Simple macro for three strings concatenation.
	* \detail Used in SIM_SCGC5 register settings.
	* SIM_SCGC5_GLUE(sim,port,mask) => simportmask
	*/
	#define SIM_SCGC5_GLUE(sim, port, mask) 	(sim##port##mask)

	/*! Turn on the clock for a specific port in SIM->SCGC5 register.
	* \detail SIM_SCGC5_SET(port) => SIM->SCGC5 |= SIM_SCGC5_PORTport_MASK
	*/
	#define SIM_SCGC5_SET(port)								SIM->SCGC5 |= SIM_SCGC5_GLUE(SIM_SCGC5_PORT, port, _MASK)
	
	/*! Turn off the clock for a specific port in SIM->SCGC5 register.
	* \detail SIM_SCGC5_CLEAR(port) => SIM->SCGC5 &= ~SIM_SCGC5_PORTport_MASK
	*/
	#define SIM_SCGC5_CLEAR(port)							SIM->SCGC5 &= ~SIM_SCGC5_GLUE(SIM_SCGC5_PORT, port, _MASK)
	
	/*! Set a pin on specified port.
	* \detail FPT_SET(port, pin) => FPTport -> PSOR = (1UL << pin)
	*/
	#define FPT_SET(port, pin)								GLUE(FPT, port) -> PSOR = (1UL<<pin)
	
	/*! Clear a pin on specified port.
	* \detail FPT_CLEAR(port, pin) => FPTport -> PCOR = (1UL << pin)
	*/
	#define FPT_CLEAR(port, pin)							GLUE(FPT, port) -> PCOR = (1UL<<pin)
	
	/*! Set a pin on specified port as a GPIO pin.
	* \detail PORT_PCR_MUX_GPIO(port, pin) => PORTport -> PCR[pin] &= ~PORT_PCR_MUX_MASK;
	*																					PORTport -> PCR[pin] |= PORT_PCR_MUX(1)
	*/
	#define PORT_PCR_MUX_GPIO(port, pin) 			GLUE(PORT, port) -> PCR[pin] &= ~PORT_PCR_MUX_MASK; \
																							GLUE(PORT, port) -> PCR[pin] |= PORT_PCR_MUX(1)
																							
	/*! Set a pin on specified port as an input.
	* \detail PIN_INPUT(port, pin) => FPTport -> PDDR &= ~(1UL<<pin)
	*/
	#define PIN_INPUT(port, pin)							GLUE(FPT, port) -> PDDR &= ~(1UL<<pin)
	
	/*! Set a pin on specified port as an output.
	* \detail PIN_OUTPUT(port, pin) => FPTport -> PDDR |= (1UL<<pin)
	*/
	#define PIN_OUTPUT(port, pin)							GLUE(FPT, port) -> PDDR |= (1UL<<pin)
	
	/*! Enable pull resistor on a specified port and pin.
	* \detail PIN_PULL_ENABLE(port, pin) => PORTport -> PCR[pin] |= PORT_PCR_PE_MASK
	*/
	#define PIN_PULL_ENABLE(port, pin)				GLUE(PORT, port) -> PCR[pin] |= PORT_PCR_PE_MASK
	
	/*! Disable pull resistor on a specified port and pin.
	* \detail PIN_PULL_DISABLE(port, pin) => PORTport -> PCR[pin] &= ~PORT_PCR_PE_MASK
	*/
	#define PIN_PULL_DISABLE(port, pin)				GLUE(PORT, port) -> PCR[pin] &= ~PORT_PCR_PE_MASK
	
	/*! Set a pull up resistor on a specified port and pin.
	* \detail PIN_PULL_UP(port, pin) => PORTport -> PCR[pin] |= PORT_PCR_PS_MASK
	*/
	#define PIN_PULL_UP(port, pin)						GLUE(PORT, port) -> PCR[pin] |= PORT_PCR_PS_MASK
	
	/*! Set a pull down resistor on a specified port and pin.
	* \detail PIN_PULL_DOWN(port, pin) => PORTport -> PCR[pin] &= ~PORT_PCR_PS_MASK
	*/
	#define PIN_PULL_DOWN(port, pin)					GLUE(PORT, port) -> PCR[pin] &= ~PORT_PCR_PS_MASK
	
	/*! Enable IRQ and set a trigger on a specified port and pin.
	* \detail PIN_IRQ_ENABLE(port, pin, edge) => PORTport -> PCR[pin] |= PORT_PCR_IRQC(edge)
	*/
	#define PIN_IRQ_ENABLE(port, pin, edge) 	GLUE(PORT, port) -> PCR[pin] |= PORT_PCR_IRQC(edge)
	
	/*! Disable IRQ on a specified port and pin.
	* \detail PIN_IRQ_DISABLE(port, pin) => PORTport -> PCR[pin] &= ~PORT_PCR_IRQC_MASK
	*/
	#define PIN_IRQ_DISABLE(port, pin) 				GLUE(PORT, port) -> PCR[pin] &= ~PORT_PCR_IRQC_MASK

	/*! Check if the interrupt request came from a specified port and pin.
	* \detail PIN_IRQ_SOURCE(port, pin) => (PORTport -> PCR[pin] & PORT_PCR_ISF_MASK)
	*/
	#define PIN_IRQ_SOURCE(port, pin)					(GLUE(PORT, port) -> PCR[pin] & PORT_PCR_ISF_MASK)
	
	/*! Clear the interrupt flag on a specified port and pin.
	* \detail PIN_IRQ_CLEAR_FLAG(port, pin) => PORTport -> PCR[pin] |= PORT_PCR_ISF_MASK
	*/
	#define PIN_IRQ_CLEAR_FLAG(port, pin)			GLUE(PORT, port) -> PCR[pin] |= PORT_PCR_ISF_MASK	
	//!@}
	
	/*! \c CE pin state flag. */
	static _Bool CE_state;
	/*! \c CSN pin state flag. The initial value is \c '1' */
	static _Bool CSN_state=1;
	
	/*! Set or clear the \c CE pin. 
	* \param setClear - \c '1': set the \c CE pin, \c '0': clear the \c CE pin;
	* \sa pin_CSN()
	*/
	void pin_CE(_Bool setClear);
	
	/*! Set or clear the \c CSN pin. 
	* \param setClear - \c '1': set the \c CSN pin, \c '0': clear the \c CSN pin;
	* \sa pin_CE()
	*/
	void pin_CSN(_Bool setClear);
	
	/*! Initialize the pins. 
	* \detail The function initializes the pins according to *PIN SETTINGS* in \c 'pinManagement.h'.
	* \sa pinManagement.h
	*/
	void pin_Init(void);

#endif
