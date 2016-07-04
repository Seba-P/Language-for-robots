/*! \brief The header file with \b Language \b for \b robots interface. 
*	\file lang4robots.h
*	\author \c Sebastian \c Pisklak & \c Jakub \c Olak
*
* This file includes declaration of all instructions, commands, language functions and recommended variables for multi-module communication.
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

#ifndef LANG4ROBOTS_H
	#define LANG4ROBOTS_H
	
	#include "MKL46Z4.h"
	#include "nRF24L01P.h"
	#include "pinManagement.h"
	#include "SPI.h"
	#include "delay.h"
	#include "nRF24.h"
	
	/*! \name LANGUAGE DEFINES
	*  Some defines used by the interface.
	*  @{
	*/
	
	/*! Number of commands used by end user.
	* \warning Do not forget to modify this constant with the appropriate value.
	*/
	#define COMMANDS_NR				3
	
	/******************
	* LANGUAGE DEFINES
	******************/
	/*! Command type enumeration.
	* \detail Fill the enumerators names according to functions names in \c 'handlersArray'. All functions used in \c 'handlersArray' must have their corresponding enumerator in \c 'CommandType' enumeration type.
	* \warning IT IS VITAL TO FILL THE ENUMERATORS IN THE SAME ORDER AS COMMAND HANDLERS IN \c 'handlersArray'! Otherwise, the command names does not match their functions and executing a specific command may call a different function.
	* \sa handlersArray
	*/
	enum CommandType{
		com0,	//!< command 0
		com1,	//!< command 1
		com2	//!< command 2
	};
	
	/*! Command handler type definition.
	* \note You may adjust the functions type according to your needs, but remember that all functions handlers stored in \c 'handlersArray' must have the same type.
	* \sa commandHandlersArray, handlersArray
	*/
	typedef uint32_t (*commandHandler)(uint32_t); 
	
	/*! Command handlers array type definition.
	* \detail The \c 'commandHandlersArray' members will have the same type as defined by the \c 'commandHandler'.
	* \sa commandHandler, handlersArray
	*/
	typedef commandHandler commandHandlersArray[];
	//!@}
	
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
	uint8_t lang4robots_sendCommand(uint8_t* addr, uint8_t comm);
	
	/*! Receive command via the radio module.
	* \param dataPipe	- data pipe number, from which the data should be received;
	* \return number of command received;
	* \sa lang4robots_sendCommand(),lang4robots_executeCommand()
	*/
	uint8_t lang4robots_receiveCommand(uint8_t dataPipe);
	
	/*! Execute the command received via the radio module.
	* \param comm	- command number to execute; the function of that number in \c 'handlersArray' will be executed;
	* \param param	- the argument parameter given to the executed function;
	* \return the return value of the executed function;
	* \sa lang4robots_sendCommand(),lang4robots_receiveCommand(), handlersArray
	*/
	uint32_t lang4robots_executeCommand(uint8_t comm, uint32_t param);
	
	/*! Initialize all modules needed.
	* \detail This function initializes the pins, nRF24 and SPI modules.
	* \note Make sure to check if all 'init' functions are configured properly.
	* \return \c '0';
	* \sa	nRF24_init(), pin_Init(), spi1init()
	*/
	uint8_t lang4robots_init(void);
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
	uint32_t fun0(uint32_t comm);
	
	/*! Function 1 handler.
	* \param comm;	
	* \return comm;
	*/
	uint32_t fun1(uint32_t comm);
	
	/*! Function 2 handler.
	* \param comm;	
	* \return comm;
	*/
	uint32_t fun2(uint32_t comm);
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
	extern enum CommandType command;
	
	/*! The array of function handlers.
	* \detail Fill the written functions names in the specified order. All functions used in \c 'handlersArray' must have their corresponding enumerator in \c 'CommandType' enumeration type.
	* \warning IT IS VITAL TO FILL THE ARRAY IN THE SAME ORDER AS GOES IN \c 'enum \c CommandType'! Otherwise, the command names does not match their functions and executing a specific command may call a different function.
	* 
	* The \c 'handlersArray' size must not exceed the number defined by the \b 'COMMANDS_NR' constant.
	* \sa CommandType,commandHandlersArray
	*/
	extern commandHandlersArray handlersArray;
	
	/*! The ACK enabled/disabled flag.
	* \detail \c '1' - send command with Auto ACK feature enabled, \c '0' - send command with Auto ACK feature disabled;
	*/
	extern _Bool ACKenabled;
	//!@}
	
#endif
