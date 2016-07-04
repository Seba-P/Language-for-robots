/*! \brief The source file with delay loops definition.
* \file delay.c
*	\author \c Sebastian \c Pisklak & \c Jakub \c Olak
*
* This file contains definition of delay loops used in other modules.
*
* \b Copyright: Sebastian Pisklak & Jakub Olak
*/

#include "delay.h"

/*! Wait for time given in milliseconds.
* \param value	- amount of milliseconds delay;
* \note Remember to modify the \b F_CPU_DEF in delay.h with the appropriate core clock frequency. Otherwise, the time of delay loop execution may be different than expected.
* \sa delay_us()
*/
void delay_ms(uint32_t value){
	uint32_t delay, x;
	
	for(x=0; x < value; x++){
		for(delay=0; delay < F_CPU_DEF/1000/5; delay++){;}
	}
}

/*! Wait for time given in microseconds.
* \param value	- amount of microseconds delay;
* \note Remember to modify the \b F_CPU_DEF in delay.h with the appropriate core clock frequency. Otherwise, the time of delay loop execution may be different than expected.
* \sa delay_ms()
*/
void delay_us(uint32_t value){
	uint32_t delay, x;
	
	for(x=0; x < value; x++){
		for(delay=0; delay < F_CPU_DEF/1000000/5; delay++){;}
	}
}
