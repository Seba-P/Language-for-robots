/*! \brief The header file with delay loops declaration.
* \file delay.h
*	\author \c Sebastian \c Pisklak & \c Jakub \c Olak
*
* This file contains declaration of delay loops used in other modules.
*
* \b Copyright: Sebastian Pisklak & Jakub Olak
*/

#ifndef delay_h
	#define delay_h

	#include "MKL46Z4.h"   /* Device header */	

	/*!Core clock frequency.
	*
	* \warning In order to provide correct delay loops, modify this constant with appropriate value in Hz.
	*/
	#define F_CPU_DEF 48000000

	/*! Wait for time given in milliseconds.
	* \param value	- amount of milliseconds delay;
	* \note Remember to modify the \b F_CPU_DEF in delay.h with the appropriate core clock frequency. Otherwise, the time of delay loop execution may be different than expected.
	* \sa delay_us()
	*/
	void delay_ms(uint32_t value);
	
	/*! Wait for time given in microseconds.
	* \param value	- amount of microseconds delay;
	* \note Remember to modify the \b F_CPU_DEF in delay.h with the appropriate core clock frequency. Otherwise, the time of delay loop execution may be different than expected.
	* \sa delay_ms()
	*/
	void delay_us(uint32_t value);

#endif
