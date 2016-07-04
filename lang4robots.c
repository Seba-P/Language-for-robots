/*! \brief The source file with \b Language \b for \b robots interface. 
*	\file lang4robots.c
*	\author \c Sebastian \c Pisklak & \c Jakub \c Olak
*
* This file includes definition of all instructions, commands, language functions and recommended variables for multi-module communication.
*	The Language 4 Robots project has been designed (...)
*
*	\b HOW \b TO \b DESIGN \b YOUR \b OWN \b LANGUAGE?
*
*	\e #lang4robots.h:
*	- add language instructions you wish to implement (up to 256) simply by writing their declarations in *COMMAND HANDLERS* section. You may rename they as you wish.
*	\warning All your instructions have to be the same type as defined \c commandHandler (C language restrictions) - by default it's \c 'uint32_t \c fun(uint32_t)', but you may adjust them according to your needs.
*	\par Remember to obey this rule, otherwise it may cause malfuction.
*	- modify \c 'enum \c CommandType' so that its enumerated members names fit your instructions name (in the same order!). It is recommended to start with first member's value = \c '0' and to name the members in the same order as your instructions name in *COMMAND HANDLERS* section without changing their values.
*	- modify the defined value \c COMMANDS_NR with the number of your language instructions
*	- optional: if you like, add more \c CommandType variables or declare more handlersArrays
*
*	\e #lang4robots.c:
*	- write your instructions functionality in the same order as declared in \c 'lang4robots.h'
*	-	initialize your \c handlerArray(s) with written functions pointers. IT IS VITAL TO FILL THE ARRAY IN THE SAME ORDER AS GOES IN \c 'enum \c CommandType'! Otherwise, the command names does not match their functions and executing a specific command may call a different function.
*
*	For more information check the project documentation.
*
* \b Copyright: Sebastian Pisklak & Jakub Olak
*/

#include "lang4robots.h"
#include "slcd.h"

/*! \name INTERFACE FUNCTIONS
	* The set of language functions provided with the \b Language \b for \b robots library.
	*  @{
	*/
	/*********************
	* INTERFACE FUNCTIONS
	*********************/
	/*! Send command via the radio module.
	* \param addr	- destination radio module address; \c addr is a pointer to the LSByte of the address, so remember to fill the address array in the appropriate order;
	* \param comm	- command number to send; you may use a \c CommandType enumerator instead of a direct value;
	* \return status of the operation: \c '1' - transmission successful, \c '0' - error in the transmission;
	* \sa lang4robots_receiveCommand(),lang4robots_executeCommand()
	*/
uint8_t lang4robots_sendCommand(uint8_t* addr, uint8_t comm){
	uint8_t status=1;
	
	pin_CE(LOW);
	delay_us(10);
	nRF24_setTXaddr(addr);
	if(ACKenabled){
		nRF24_setRXaddr(0,addr); //required for ACK
		nRF24_sendData(&comm,1);
	}else{
		nRF24_sendDataNOACK(&comm,1);
	}
	nRF24_modeTX();
	pin_CE(HIGH);
	
	return status;
}

/*! Receive command via the radio module.
	* \param dataPipe	- data pipe number, from which the data should be received;
	* \return number of command received;
	* \sa lang4robots_sendCommand(),lang4robots_executeCommand()
	*/
uint8_t lang4robots_receiveCommand(uint8_t dataPipe){
	uint8_t comm;//,statusReg;
	
	//statusReg=nRF24_getStatus();
	//if((statusReg & RX_P_NO(7))==RX_P_NO(dataPipe)){
		nRF24_receiveData(&comm, 1);
	//}else{
		//return 0xFF; //error avoidance
	//}
	
	return comm;
}

/*! Execute the command received via the radio module.
	* \param comm	- command number to execute; the function of that number in \c 'handlersArray' will be executed;
	* \param param	- the argument parameter given to the executed function;
	* \return the return value of the executed function;
	* \sa lang4robots_sendCommand(),lang4robots_receiveCommand(), handlersArray
	*/
uint32_t lang4robots_executeCommand(uint8_t comm, uint32_t param){
	if(comm>COMMANDS_NR){
		return 0xFF; //error avoidance
	}
		
	return handlersArray[comm](param);
}

/*! Initialize all modules needed.
	* \detail This function initializes the pins, nRF24 and SPI modules.
	* \note Make sure to check if all 'init' functions are configured properly.
	* \return \c '0';
	* \sa	nRF24_init(), pin_Init(), spi1init()
	*/
uint8_t lang4robots_init(void){
	pin_Init();
	spi1init();
	nRF24_init();
	
	return 0;
}
//!@}
	
	/*! \name COMMAND HANDLERS
	*  The set of application specified command handlers written by end user.
	*  @{
	*/
	/******************
	* COMMAND HANDLERS
	******************/
	/*! Function 0 handler.
	* \param comm;	
	* \return comm;
	*/
uint32_t fun0(uint32_t comm){
	slcdErr(0);
	return comm;
}

/*! Function 1 handler.
	* \param comm;	
	* \return comm;
	*/
uint32_t fun1(uint32_t comm){
	slcdErr(1);
	return comm;
}

/*! Function 2 handler.
	* \param comm;	
	* \return comm;
	*/
uint32_t fun2(uint32_t comm){
	slcdErr(2);
	return comm;
}
//!@}
	
	/*! \name COMMAND VARIABLES AND HANDLERS ARRAY
	*  The command variables and array of handlers customized by end user.
	*  @{
	*/
	/**************************************
	* COMMAND VARIABLES AND HANDLERS ARRAY 
	**************************************/
	/*! The variable used to store command values.
	* \detail Usage of \c CommandType variables instead of direct values is recommended. You may define as many \c CommandType variables as you need.
	* \sa CommandType
	*/
enum CommandType command;

	/*! The array of function handlers.
	* \detail Fill the written functions names in the specified order. All functions used in \c 'handlersArray' must have their corresponding enumerator in \c 'CommandType' enumeration type.
	* \warning IT IS VITAL TO FILL THE ARRAY IN THE SAME ORDER AS GOES IN \c 'enum \c CommandType'! Otherwise, the command names does not match their functions and executing a specific command may call a different function.
	* 
	* The \c 'handlersArray' size must not exceed the number defined by the \b 'COMMANDS_NR' constant.
	* \sa CommandType,commandHandlersArray
	*/
commandHandlersArray handlersArray={
	fun0,
	fun1,
	fun2
};
	
	/*! The ACK enabled/disabled flag.
	* \detail \c '1' - send command with Auto ACK feature enabled, \c '0' - send command with Auto ACK feature disabled;
	*/
_Bool ACKenabled=1;
//!@}
