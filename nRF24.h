/*! \brief The header file with \b nRF24L01+ interface declaration. 
*	\file nRF24.h
*	\author \c Sebastian \c Pisklak & \c Jakub \c Olak
*	
* This file contains declaration of all variables, low-level and high-level operations used to interface with \b nRF24L01+ module. 
*
* \b Copyright: Sebastian Pisklak & Jakub Olak
*/

#ifndef nRF24_H
  #define nRF24_H

  #include "MKL46Z4.h"
  #include "SPI.h"
  #include "nRF24L01P.h"
	#include "pinManagement.h"
	#include "delay.h"
	
	/*! \name DEFAULT CONFIGURATION
	*  The default configuration variables.
	*  @{
	*/
  /***********************
  * DEFAULT CONFIGURATION
  ***********************/
  /*
  CONFIG_INIT			(MASK_RX_DR | MASK_TX_DS | MASK_MAX_RT | EN_CRC | CRC0 | PWR_UP | PRIM_RX) //enabled RX_DR, TX_DS and MAX_RT interrupts; enabled CRC (2 bytes); device powerred up and in RX mode
  EN_AA_INIT			(ENAA_P5 | ENAA_P4 | ENAA_P3 | ENAA_P2 | ENAA_P1 | ENAA_P0) //enabled Auto ACK on all pipes
  EN_RXADDR_INIT	(ERX_P5 | ERX_P4 | ERX_P3 | ERX_P2 | ERX_P1 | ERX_P0) //enable all data pipes
  SETUP_AW_INIT		AW(3) //RX/TX address width - 5 bytes		
  SETUP_RETR_INIT	(ARD(1) | ARC(15)) //Automatic Retransmission Delay - 500us; Automatic Retransmission Count - up to 15 times (maximum available value) 
  RF_CH_INIT			RF_CH_MASK(0) //RF Channel - 0 (2.4GHz)
  RF_SETUP_INIT		(RF_DR_LOW | RF_PWR(3)) //RF settings: data rate - 250Kbps, output power - 0dBm 
  STATUS_INIT			(RX_DR | TX_DS | MAX_RT) //cleared interrupt flags
  RX_ADDR_P0_INIT	0xD5D5D5D5D5 //default RX address for data pipe 0
  RX_ADDR_P1_INIT	0xC3C3C3C3C1 //default RX address for data pipe 1
  RX_ADDR_P2_INIT	0xC2 //default RX address for data pipe 2 (only LSByte, first 4 MSBytes are common for data pipes 1-5)
  RX_ADDR_P3_INIT	0xC3 //default RX address for data pipe 3
  RX_ADDR_P4_INIT	0xC4 //default RX address for data pipe 4
  RX_ADDR_P5_INIT	0xC5 //default RX address for data pipe 5
  TX_ADDR_INIT		0xD5D5D5D5D5 //default TX address
  RX_PW_P0_INIT		RX_PW(1) //default payload for data pipe 0 - 1 byte
  RX_PW_P1_INIT		RX_PW(1) //default payload for data pipe 1 - 1 byte
  RX_PW_P2_INIT		RX_PW(1) //default payload for data pipe 2 - 1 byte
  RX_PW_P3_INIT		RX_PW(1) //default payload for data pipe 3 - 1 byte
  RX_PW_P4_INIT		RX_PW(1) //default payload for data pipe 4 - 1 byte
  RX_PW_P5_INIT		RX_PW(1) //default payload for data pipe 5 - 1 byte
  DYN_PD_INIT			0 //Dynamic Payload Length disabled for all data pipes
  FEATURE_INIT		EN_DYN_ACK //enabled W_TX_PAYLOAD_NOACK command; disabled ACK Payload and Dynamic Payload Length
  */
  //!@}

	/*! \name GLOBAL VARIABLES
	*  The global variables and arrays used in different interface functions.
	*  @{
	*/
  /******************
  * GLOBAL VARIABLES 
  ******************/
  volatile extern uint8_t TX_PLD[32]; //!< TX Payload buffer (up to 32 bytes). May be used to store data for transmission.
  volatile extern uint8_t RX_PLD[32]; //!< RX Payload buffer (up to 32 bytes). May be used to store received data.
  volatile extern uint8_t ACK_PLD[32]; //!< ACK Payload buffer (up to 32 bytes). May be used to store data which would be attached to ACK packet.
	
  volatile extern uint8_t _RX_ADDR_P0[5]; //!< RX Address for data pipe 0 (up to 5 bytes). The address bytes' order is from LSByte (_RX_ADDR_P0[0]) to MSByte (_RX_ADDR_P0[4]).
  volatile extern uint8_t _RX_ADDR_P1[5]; //!< RX Address for data pipe 1 (up to 5 bytes). The address bytes' order is from LSByte (_RX_ADDR_P1[0]) to MSByte (_RX_ADDR_P1[4]).
  volatile extern uint8_t _RX_ADDR_P2; //!< The LSByte for data pipe 2 RX Address. First 4 MSBytes are the same as for data pipe 1.
  volatile extern uint8_t _RX_ADDR_P3; //!< The LSByte for data pipe 3 RX Address. First 4 MSBytes are the same as for data pipe 1.
  volatile extern uint8_t _RX_ADDR_P4; //!< The LSByte for data pipe 4 RX Address. First 4 MSBytes are the same as for data pipe 1.
  volatile extern uint8_t _RX_ADDR_P5; //!< The LSByte for data pipe 5 RX Address. First 4 MSBytes are the same as for data pipe 1.
  volatile extern uint8_t _TX_ADDR[5]; //!< TX Address (up to 5 bytes). The address bytes' order is from LSByte (_TX_ADDR[0]) to MSByte (_TX_ADDR[4]).
																/*!< \note In order to receive ACK packet, set the same address for data pipe 0 before start of the transmission. */
	//!@}	
	/*! \name LOW-LEVEL INSTRUCTIONS
	*  Some low-level instructions used by interface functions.
	*  @{
	*/
  /************************
  * LOW-LEVEL INSTRUCTIONS
  ************************/
	
	/*! Send an instruction to the module.
	* \param com - module instruction value (use instruction mnemonics provided with \c 'nRF24L01P.h' file);
	* \return module instruction value passed to the function;
	* \sa nRF24L01P.h,nRF24_readRegister(),nRF24_writeRegister()
	*/
	uint8_t nRF24_sendCommand(const uint8_t com);
  
	/*! Write to the module register.
	* \param reg - module register address (use register mnemonics provided with \c 'nRF24L01P.h' file);
	* \param val - an address for first value to write to the module register;
	* \param len - amount of bytes to write to the module register;
	* \return last value written to the register from \c val array;
	* \sa nRF24L01P.h,nRF24_readRegister(),nRF24_sendCommand()
	*/
	uint8_t nRF24_writeRegister(const uint8_t reg, uint8_t* val, uint8_t len);
	
	/*! Read from the module register.
	* \param reg - module register address (use register mnemonics provided with \c 'nRF24L01P.h' file);
	* \param dest - a pointer to first address, where the read data will be written;
	* \param len - amount of bytes to read from the module register;
	* \return last value read from the module register;
	* \sa nRF24L01P.h,nRF24_readRegister(),nRF24_sendCommand()
	*/
  uint8_t nRF24_readRegister(const uint8_t reg, uint8_t* val, uint8_t len);
  //!@}

	/*! \name INTERFACE FUNCTIONS
	*  The interface functions. Use of this set of instructions is recommended for end users.
	*  @{
	*/
  /*********************
  * INTERFACE FUNCTIONS
  *********************/
	/*! Get \c STATUS register value. 
	* \return \c STATUS register value;
	* \sa nRF24L01P.h,nRF24_getFIFOstatus()
	*/
	uint8_t nRF24_getStatus(void);

	/*! Get \c FIFO_STATUS register value. 
	* \return \c FIFO_STATUS register value;
	* \sa nRF24L01P.h,nRF24_getStatus()
	*/
	uint8_t nRF24_getFIFOstatus(void);
	
	/*! Power down the module. 
	* \return \c CONFIG register value;
	* \sa nRF24L01P.h,nRF24_modeRX(),nRF24_modeTX()
	*/
	uint8_t nRF24_powerDown(void);
		
	/*! Switch to RX mode. 
	* \return \c CONFIG register value;
	* \sa nRF24L01P.h,nRF24_modeTX(),nRF24_powerDown()
	*/
	uint8_t nRF24_modeRX(void);
		
	/*! Switch to TX mode. 
	* \return \c CONFIG register value;
	* \sa nRF24L01P.h,nRF24_modeRX(),nRF24_powerDown()
	*/
	uint8_t nRF24_modeTX(void);

	/*! Initialize the module with the default configuration. 
	* \detail The function updates all module registers with the *DEFAULT CONFIGURATION* registers. You may modify them according to your needs.
	* \return \c STATUS register value;
	* \sa nRF24L01P.h
	*/
	uint8_t nRF24_init(void);

	/*! Set TX Address.
	* \warning The module must be in standby I mode and \c CE pin must be low. Rewriting the TX Address during a transmission is prohibited.
	* \param txAddr - a pointer to the LSByte of the address. You may use the \c '_TX_ADDR' array to store your address;
	* \return the LSByte of the address;
	* \sa nRF24_setRXaddr(),_TX_ADDR
	*/
	uint8_t nRF24_setTXaddr(uint8_t* txAddr);

	/*! Set RX Address for specific data pipe.
	* \warning The module must be in standby I mode and \c CE pin must be low. Rewriting the RX Address during a transmission is prohibited.
	* \param dataPipe - number of data pipe for which the address will be set. The function knows how many bytes should be written depending on the data pipe number;
	* \param rxAddr - a pointer to the LSByte of the address. You may use the \c '_RX_ADDR_P0-1' arrays or \c '_RX_ADDR_P2-5' variables to store your address.
	* \return the LSByte of the address;
	* \sa nRF24_setTXaddr(),_RX_ADDR
	*/
	uint8_t nRF24_setRXaddr(uint8_t dataPipe, uint8_t* rxAddr);
	
	/*! Interrupt settings.
	* \note The IRQ pin must be connected to the MCU and the module should not be in any transmission mode during IRQ configuration.
	* \par The interrupt flags in \c STATUS register should be cleared before enabling IRQ's.
	* \param irqVal - pass the combination of interrupt mnemonics (TX_DS, RX_DR, MAX_RT) which will be enabled/disabled. You may enable/disable more than one IRQ at a time by summing mnemonics with the '|' operator;
	* \param irqEn - \c '1': enable IRQ's with \c irqVal, \c '0': disable IRQ's with \c irqVal;
	* \return \c CONFIG register value;
	* \sa 
	*/
	uint8_t nRF24_enDisIRQ(uint8_t irqVal, _Bool irqEn);
	
	/*! CRC settings.
	* \note The module should not be in any transmission mode during CRC configuration.
	* \param crc - \c '0': no CRC, \c '1': 1 byte CRC, \c default: 2 byte CRC;
	* \return \c CONFIG register value;
	* \sa 
	*/
	uint8_t nRF24_setCRC(uint8_t crc);

	/*! Auto ACK settings.
	* \note The module should not be in any transmission mode during Auto ACK configuration.
	* \param AAval - pass the combination of EN_AA mnemonics (ENAA_P0-5) which will be enabled/disabled. You may enable/disable Auto ACK on more than one data pipe at a time by summing mnemonics with the '|' operator;
	* \param AAen - \c '1': enable Auto ACK with \c AAval, \c '0': disable Auto ACK with \c AAval;
	* \return \c EN_AA register value;
	* \sa nRF24_enDisDataPipe(),nRF24_enDisDynPayLen(),nRF24_enDisDynACK(),nRF24_enDisACKpayload()
	*/
	uint8_t nRF24_enDisAutoACK(uint8_t AAval, _Bool AAen);
		
	/*! Data pipes settings.
	* \note The module should not be in any transmission mode during data pipe configuration.
	* \param dataPipeVal - pass the combination of EN_RXADDR mnemonics (ERX_P0-5) which will be enabled/disabled. You may enable/disable more than one data pipe at a time by summing mnemonics with the '|' operator;
	* \param dataPipeEn - \c '1': enable data pipes with \c dataPipeVal, \c '0': disable data pipes with \c dataPipeVal;
	* \return \c EN_RXADDR register value;
	* \sa nRF24_enDisAutoACK(),nRF24_enDisDynPayLen(),nRF24_enDisDynACK(),nRF24_enDisACKpayload()
	*/
	uint8_t nRF24_enDisDataPipe(uint8_t dataPipeVal, _Bool dataPipeEn);

	/*! Set Address Width for data pipes.
	* \note The module should not be in any transmission mode during Address Width configuration.
	* \param addrW - \c '5': 5 bytes, \c '4': 4 bytes, \c '3': 3 bytes, \c default: 5 bytes;
	* \return \c SETUP_AW register value;
	* \sa 
	*/
	uint8_t nRF24_setAddresWidth(uint8_t addrW);

	/*! Set Automatic Retransmission Delay.
	* \note The module should not be in any transmission mode during Automatic Retransmission Delay configuration.
	* \param ard - \c '15': 4000 us, \c '14': 3750 us, ... , \c '1': 500 us, \c '0': 250 us, \c default: 4000 us;
	* \return \c SETUP_RETR register value;
	* \sa nRF24_setAutoRetranCount()
	*/
	uint8_t nRF24_setAutoRetranDelay(uint8_t ard);

	/*! Set Automatic Retransmission Count.
	* \param ard - \c '15': up to 15 times, \c '14': up to 14 times, ... , \c '1': up to 1 time, \c '0': no auto retransmissions, \c default: up to 15 times;
	* \return \c SETUP_RETR register value;
	* \sa nRF24_setAutoRetranDelay()
	*/
	uint8_t nRF24_setAutoRetranCount(uint8_t arc);

	/*! Set RF Channel.
	* \param channel - number of RF Channel on which the module will operate;
	* \return \c RF_CH register value;
	* \sa nRF24_setRFdataRate(),nRF24_setRFoutputPower()
	*/
	uint8_t nRF24_setRFchannel(uint8_t channel);

	/*! Set RF Data Rate.
	* \param dataRate - \c '1': 1 Mbps, \c '0': 250 Kbps, \c default: 2 Mbps;
	* \return \c RF_SETUP register value;
	* \sa nRF24_setRFchannel(),nRF24_setRFoutputPower()
	*/
	uint8_t nRF24_setRFdataRate(uint8_t dataRate);

	/*! Set RF Output Power.
	* \param power - \c '2': -6 dBm, \c '1': -12 dBm, \c '0': -18 dBm, \c default: 0 dBm;
	* \return \c RF_SETUP register value;
	* \sa nRF24_setRFdataRate(),nRF24_setRFchannel()
	*/
	uint8_t nRF24_setRFoutputPower(uint8_t power);

	/*! Start PLL carrier test.
	* \detail Not implemented yet.
	*/
	uint8_t nRF24_PLLcarrierTest(void);

	/*! Get Packet Loss Counter.
	* \return Packet Loss Counter value;
	* \sa nRF24_getPacketRetranCount()
	*/
	uint8_t nRF24_getPacketLossCount(void);

	/*! Get Packet Retransmit Counter.
	* \return Packet Retransmit Counter value;
	* \sa nRF24_getPacketLossCount()
	*/
	uint8_t nRF24_getPacketRetranCount(void);

	/*! Set payload width for specific data pipe.
	* \param dataPipe - number of data pipe the payload width will be set (0-5);
	* \param payWidth - amount of bytes in the payload for given data pipe (0-32);
	* \return if payWidth and dataPipe values are in correct value range, the function returns payload width value. Otherwise, the \c '0xFF' is returned;
	* \sa nRF24_getRXpayWidth()
	*/
	uint8_t nRF24_setRXpayloadWidth(uint8_t dataPipe, uint8_t payWidth);
	
	/*! Dynamic Payload Length settings.
	* \note The module should not be in any transmission mode during Dynamic Payload Length configuration.
	* \warning In order to enable DPL on data pipes, Auto ACK must be enabled for them (EN_AA configuration).
	* \param dataPipeVal - pass the combination of DYN_PD mnemonics (DPL_P0-5) which will be enabled/disabled. You may enable/disable DPL on more than one data pipe at a time by summing mnemonics with the '|' operator;
	* \param DPLenable - \c '1': enable DPL on data pipes with \c dataPipeVal, \c '0': disable DPL on data pipes with \c dataPipeVal;
	* \return \c DYN_PD register value;
	* \sa nRF24_enDisDataPipe(),nRF24_enDisAutoACK(),nRF24_enDisDynACK(),nRF24_enDisACKpayload()
	*/
	uint8_t nRF24_enDisDynPayLen(uint8_t dataPipeVal, _Bool DPLenable);
	
	/*! ACK Payload settings.
	* \note The module should not be in any transmission mode during ACK Payload configuration.
	* \warning If ACK packet payload is activated, ACK packets have dynamic payload length and Dynamic Payload Length feature should be enabled for data pipe 0 on the PTX and PRX devices (EN_DPL,ENAA_P0,DPL_P0). This is to ensure that they receive ACK packets with payloads. Also set the correct ARD value..
	* \param enDis - \c '1': enable ACK Payload, \c '0': disable ACK Payload;
	* \return \c FEATURE register value;
	* \sa nRF24_enDisDataPipe(),nRF24_enDisDynPayLen(),nRF24_enDisDynACK(),nRF24_enDisAutoACK()
	*/
	uint8_t nRF24_enDisACKpayload(_Bool enDis);

	/*! Dynamic ACK settings.
	* \note The module should not be in any transmission mode during Dynamic ACK configuration.
	* \param enDis - \c '1': enable Dynamic ACK, \c '0': disable Dynamic ACK;
	* \return \c FEATURE register value;
	* \sa nRF24_enDisDataPipe(),nRF24_enDisDynPayLen(),nRF24_enDisAutoACK(),nRF24_enDisACKpayload()
	*/
	uint8_t nRF24_enDisDynACK(_Bool enDis);

	/*! Set TX Payload Reuse.
	* \note The module should not be in any transmission mode during TX Payload Reuse configuration.
	* \warning TX Payload Reuse may be set only if there was at least one packet successfully transmitted after powering up the module. 
	* \param payloadReuse - \c '1': enable TX Payload Reuse, \c '0': disable TX Payload Reuse and flush TX FIFO;
	* \return \c FIFO_STATUS register value;
	* \sa nRF24_flushTX()
	*/
	uint8_t nRF24_setTXpayloadReuse(_Bool payloadReuse);

	/*! Flush RX FIFO.
	* \return \c FIFO_STATUS register value;
	* \sa nRF24_flushTX()
	*/
	uint8_t nRF24_flushRX(void);

	/*! Flush TX FIFO.
	* \return \c FIFO_STATUS register value;
	* \sa nRF24_flushRX()
	*/
	uint8_t nRF24_flushTX(void);

	/*! Get payload width for the next RX Payload in RX FIFO.
	* \return payload width for the next RX Payload in RX FIFO;
	* \sa nRF24_setRXpayloadWidth()
	*/
	uint8_t nRF24_getRXpayWidth(void);

	/*! Send data to TX FIFO.
	* \note The module should not be in any transmission mode during writing to TX FIFO.
	* \param val - a pointer to data array;
	* \param len - amount of bytes to send (0-32). If the \c len value is greater than 32, the function will write only first 32 bytes to TX FIFO;
	* \return \c FIFO_STATUS register value;
	* \sa nRF24_sendDataNOACK(),nRF24_receiveData()
	*/
	uint8_t nRF24_sendData(uint8_t* data, uint8_t len);

	/*! Send data to TX FIFO with NOACK flag set.
	* \note The module should not be in any transmission mode during writing to TX FIFO.
	* \par In order to use this function, the Dynamic ACK must be enabled (FEATURE).
	* \param val - a pointer to data array;
	* \param len - amount of bytes to send (0-32). If the \c len value is greater than 32, the function will write only first 32 bytes to TX FIFO;
	* \return \c FIFO_STATUS register value;
	* \sa nRF24_sendData(),nRF24_receiveData(),nRF24_enDisDynACK() 
	*/
	uint8_t nRF24_sendDataNOACK(uint8_t* data, uint8_t len);

	/*! Receive data from RX FIFO.
	* \note Usage of nRF24_getRXpayWidth() function to specify the \c len value is recommended.
	* \param dest - a pointer to the destination array, where the received data will be stored;
	* \param len - amount of bytes to send (0-32). If the \c len value is greater than 32, the function will write only first 32 bytes to TX FIFO;
	* \return if there is less bytes in RX FIFO then the \c len value, the function returns \c '0xFF'. Otherwise, \c FIFO_STATUS register value is returned;
	* \sa nRF24_sendData(),nRF24_sendDataNOACK(),nRF24_getRXpayWidth()
	*/
	uint8_t nRF24_receiveData(uint8_t* dest, uint8_t len);
	//!@}

#endif
