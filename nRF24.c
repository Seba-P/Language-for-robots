/*! \brief The source file with \b nRF24L01+ interface definition. 
*	\file nRF24.c
*	\author \c Sebastian \c Pisklak & \c Jakub \c Olak
*	
* This file contains definition of all variables, low-level and high-level operations used to interface with \b nRF24L01+ module. 
*
* \b Copyright: Sebastian Pisklak & Jakub Olak
*/

#include "nRF24.h"
//#include "MKL46Z4.h"
//#include "SPI.h"
//#include "delay.h"

/*! \name DEFAULT CONFIGURATION
*  The default configuration variables.
*  @{
*/
/***********************
* DEFAULT CONFIGURATION
***********************/
static uint8_t CONFIG_INIT=			(EN_CRC | CRC0 | PWR_UP | PRIM_RX); //!< enabled RX_DR, TX_DS and MAX_RT interrupts; enabled CRC (2 bytes); device powered up and in RX mode
static uint8_t EN_AA_INIT=			(ENAA_P5 | ENAA_P4 | ENAA_P3 | ENAA_P2 | ENAA_P1 | ENAA_P0); //!< enabled Auto ACK on all pipes
static uint8_t EN_RXADDR_INIT=	(ERX_P5 | ERX_P4 | ERX_P3 | ERX_P2 | ERX_P1 | ERX_P0); //!< enabled all data pipes
static uint8_t SETUP_AW_INIT=		AW(3); //!< RX/TX address width - 5 bytes		
static uint8_t SETUP_RETR_INIT=	(ARD(1) | ARC(15)); //!< Automatic Retransmission Delay - 500us; Automatic Retransmission Count - up to 15 times (maximum available value) 
static uint8_t RF_CH_INIT=			RF_CH_MASK(0); //!< RF Channel - 0 (2.4GHz)
static uint8_t RF_SETUP_INIT=		(RF_DR_LOW | RF_PWR(3)); //!< RF settings: data rate - 250Kbps, output power - 0dBm 
static uint8_t STATUS_INIT=			(RX_DR | TX_DS | MAX_RT); //!< cleared interrupt flags
static uint8_t RX_PW_P0_INIT=		RX_PW(1); //!< default payload for data pipe 0 - 1 byte
static uint8_t RX_PW_P1_INIT=		RX_PW(1); //!< default payload for data pipe 1 - 1 byte
static uint8_t RX_PW_P2_INIT=		RX_PW(1); //!< default payload for data pipe 2 - 1 byte
static uint8_t RX_PW_P3_INIT=		RX_PW(1); //!< default payload for data pipe 3 - 1 byte
static uint8_t RX_PW_P4_INIT=		RX_PW(1); //!< default payload for data pipe 4 - 1 byte
static uint8_t RX_PW_P5_INIT=		RX_PW(1); //!< default payload for data pipe 5 - 1 byte
static uint8_t DYN_PD_INIT=			0; //!< Dynamic Payload Length disabled for all data pipes
static uint8_t FEATURE_INIT=		EN_DYN_ACK; //!< enabled W_TX_PAYLOAD_NOACK command; disabled ACK Payload and Dynamic Payload Length
//!@}

/*! \name GLOBAL VARIABLES
*  The global variables and arrays used in different interface functions.
*  @{
*/
/******************
* GLOBAL VARIABLES 
******************/

volatile uint8_t TX_PLD[32]; //!< TX Payload buffer (up to 32 bytes). May be used to store data for transmission.
volatile uint8_t RX_PLD[32]; //!< RX Payload buffer (up to 32 bytes). May be used to store received data.
volatile uint8_t ACK_PLD[32]; //!< ACK Payload buffer (up to 32 bytes). May be used to store data which would be attached to ACK packet.
	
volatile uint8_t _RX_ADDR_P0[5]={0xD5,0xD5,0xD5,0xD5,0xD5}; //!< RX Address for data pipe 0 (up to 5 bytes). The address bytes' order is from LSByte (_RX_ADDR_P0[0]) to MSByte (_RX_ADDR_P0[4]).
volatile uint8_t _RX_ADDR_P1[5]={0xC1,0xC3,0xC3,0xC3,0xC3}; //!< RX Address for data pipe 1 (up to 5 bytes). The address bytes' order is from LSByte (_RX_ADDR_P1[0]) to MSByte (_RX_ADDR_P1[4]).
volatile uint8_t _RX_ADDR_P2=0xC2; //!< The LSByte for data pipe 2 RX Address. First 4 MSBytes are the same as for data pipe 1.
volatile uint8_t _RX_ADDR_P3=0xC3; //!< The LSByte for data pipe 3 RX Address. First 4 MSBytes are the same as for data pipe 1.
volatile uint8_t _RX_ADDR_P4=0xC4; //!< The LSByte for data pipe 4 RX Address. First 4 MSBytes are the same as for data pipe 1.
volatile uint8_t _RX_ADDR_P5=0xC5; //!< The LSByte for data pipe 5 RX Address. First 4 MSBytes are the same as for data pipe 1.
volatile uint8_t _TX_ADDR[5]={0xD5,0xD5,0xD5,0xD5,0xD5}; //!< TX Address (up to 5 bytes). The address bytes' order is from LSByte (_TX_ADDR[0]) to MSByte (_TX_ADDR[4]).																									/*!< \note In order to receive ACK packet, set the same address for data pipe 0 before start of the transmission. */
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
uint8_t nRF24_sendCommand(const uint8_t com){
  pin_CSN(LOW);
	//delay_us(130); //?
  spi1_byte_send(com,0);
	delay_us(10);
  pin_CSN(HIGH);
	
	return com;
}
  
/*! Write to the module register.
* \warning Make sure the \c CE pin is low before writing to any register. The module cannot be in RX,TX or standby II mode, but may be powered down.
* \param reg - module register address (use register mnemonics provided with \c 'nRF24L01P.h' file);
* \param val - a pointer to the source array, from where the data will be written to the module register;
* \param len - amount of bytes to write to the module register;
* \return last value written to the register from \c val array;
* \sa nRF24L01P.h,nRF24_readRegister(),nRF24_sendCommand()
*/
uint8_t nRF24_writeRegister(const uint8_t reg, uint8_t* val, uint8_t len){
  int i;
	
	pin_CSN(LOW);
	//delay_us(130); //?
  spi1_byte_send(W_REGISTER | reg,0);
  for(i=0; i<len; i++){
    //delay_us(10);
		spi1_byte_send(*(val+i) , 0);
  }
	//delay_us(10);
  pin_CSN(HIGH);
	delay_us(10);

	return *(val+i-1);
}

/*! Read from the module register.
* \note In opposite to writing operations, reading from the module registers is allowed in any mode.
* \param reg - module register address (use register mnemonics provided with \c 'nRF24L01P.h' file);
* \param dest - a pointer to the destination array, where the read data will be stored;
* \param len - amount of bytes to read from the module register;
* \return last value read from the module register;
* \sa nRF24L01P.h,nRF24_readRegister(),nRF24_sendCommand()
*/
uint8_t nRF24_readRegister(const uint8_t reg, uint8_t* dest, uint8_t len){
  int i;

  pin_CSN(LOW);
	//delay_us(130);
  spi1_byte_send(R_REGISTER + reg,0);
  for(i=0; i<len; i++){
		//delay_us(10);
		*(dest+i)=spi1_read_byte(NOP);
  }
	//delay_us(10);
  pin_CSN(HIGH);
	delay_us(10);
	
	return *(dest+i-1);
}
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
uint8_t nRF24_getStatus(void){
	uint8_t statusReg;
	
	pin_CSN(0);
	statusReg=spi1_read_byte(0xFF);
	pin_CSN(1);
	
	return statusReg;
}

/*! Get \c FIFO_STATUS register value. 
* \return \c FIFO_STATUS register value;
* \sa nRF24L01P.h,nRF24_getStatus()
*/
uint8_t nRF24_getFIFOstatus(void){
	uint8_t fifo_statusReg;
	
	nRF24_readRegister(FIFO_STATUS,&fifo_statusReg,1);
	
	return fifo_statusReg;
}

/*! Power down the module. 
* \return \c CONFIG register value;
* \sa nRF24L01P.h,nRF24_modeRX(),nRF24_modeTX()
*/
uint8_t nRF24_powerDown(void){
	uint8_t configReg;

	nRF24_readRegister(CONFIG,&configReg,1);
  configReg &= ~PWR_UP;
  nRF24_writeRegister(CONFIG,&configReg,1);
  //delay_ms(5);

	return configReg;
}
	
/*! Switch to RX mode. 
* \return \c CONFIG register value;
* \sa nRF24L01P.h,nRF24_modeTX(),nRF24_powerDown()
*/
uint8_t nRF24_modeRX(void){
  uint8_t configReg;

	configReg=nRF24_powerDown();
  configReg |= PRIM_RX | PWR_UP;
  nRF24_writeRegister(CONFIG,&configReg,1); 
  delay_ms(5);
	
	return configReg;
}
	
/*! Switch to TX mode. 
* \return \c CONFIG register value;
* \sa nRF24L01P.h,nRF24_modeRX(),nRF24_powerDown()
*/
uint8_t nRF24_modeTX(void){
  uint8_t configReg;

	configReg=nRF24_powerDown();
	configReg &= ~PRIM_RX;
  configReg |= PWR_UP;
  nRF24_writeRegister(CONFIG,&configReg,1);
  delay_ms(5);
	
	return configReg;
 }

/*! Initialize the module with the default configuration. 
* \detail The function updates all module registers with the *DEFAULT CONFIGURATION* registers. You may modify them according to your needs.
* \return \c STATUS register value;
* \sa nRF24L01P.h
*/
uint8_t nRF24_init(void){
	pin_CSN(HIGH);
	delay_ms(100);
  nRF24_writeRegister(CONFIG,&CONFIG_INIT,1);
	delay_ms(5);
  nRF24_writeRegister(EN_AA,&EN_AA_INIT,1);
	nRF24_writeRegister(EN_RXADDR,&EN_RXADDR_INIT,1);
	nRF24_writeRegister(SETUP_AW,&SETUP_AW_INIT,1);
	nRF24_writeRegister(SETUP_RETR,&SETUP_RETR_INIT,1);
	nRF24_writeRegister(RF_CH,&RF_CH_INIT,1);
	nRF24_writeRegister(RF_SETUP,&RF_SETUP_INIT,1);
	nRF24_writeRegister(STATUS,&STATUS_INIT,1);
	nRF24_writeRegister(RX_PW_P0,&RX_PW_P0_INIT,1);
	nRF24_writeRegister(RX_PW_P1,&RX_PW_P1_INIT,1);
	nRF24_writeRegister(RX_PW_P2,&RX_PW_P2_INIT,1);
	nRF24_writeRegister(RX_PW_P3,&RX_PW_P3_INIT,1);
	nRF24_writeRegister(RX_PW_P4,&RX_PW_P4_INIT,1);
	nRF24_writeRegister(RX_PW_P5,&RX_PW_P5_INIT,1);
	nRF24_writeRegister(DYN_PD,&DYN_PD_INIT,1);
	nRF24_writeRegister(FEATURE,&FEATURE_INIT,1);
	nRF24_writeRegister(RX_ADDR_P0,_RX_ADDR_P0,5);
	nRF24_writeRegister(RX_ADDR_P1,_RX_ADDR_P1,5);
	nRF24_writeRegister(RX_ADDR_P2,&_RX_ADDR_P2,1);
	nRF24_writeRegister(RX_ADDR_P3,&_RX_ADDR_P3,1);
	nRF24_writeRegister(RX_ADDR_P4,&_RX_ADDR_P4,1);
	nRF24_writeRegister(RX_ADDR_P5,&_RX_ADDR_P5,1);
	nRF24_writeRegister(TX_ADDR,_TX_ADDR,5);
	//delay_ms(100);
	//pin_CE(HIGH);
	nRF24_sendCommand(FLUSH_RX);
	delay_us(10);
	nRF24_sendCommand(FLUSH_TX);
	delay_us(10);
	
  return nRF24_getStatus();
}

/*! Set TX Address.
* \warning The module must be in standby I mode and \c CE pin must be low. Rewriting the TX Address during a transmission is prohibited.
* \param txAddr - a pointer to the LSByte of the address. You may use the \c '_TX_ADDR' array to store your address;
* \return the LSByte of the address;
* \sa nRF24_setRXaddr(),_TX_ADDR
*/
uint8_t nRF24_setTXaddr(uint8_t* txAddr){
	nRF24_writeRegister(TX_ADDR,txAddr,5);
	
	return *txAddr;
}

/*! Set RX Address for specific data pipe.
* \warning The module must be in standby I mode and \c CE pin must be low. Rewriting the RX Address during a transmission is prohibited.
* \param dataPipe - number of data pipe for which the address will be set. The function knows how many bytes should be written depending on the data pipe number;
* \param rxAddr - a pointer to the LSByte of the address. You may use the \c '_RX_ADDR_P0-1' arrays or \c '_RX_ADDR_P2-5' variables to store your address.
* \return the LSByte of the address;
* \sa nRF24_setTXaddr(),_RX_ADDR
*/
uint8_t nRF24_setRXaddr(uint8_t dataPipe, uint8_t* rxAddr){
	if(dataPipe>5){
		return 0xFF; //error avoidance
	}
	
	if(dataPipe==0 || dataPipe==1){
		nRF24_writeRegister(RX_ADDR_P0+dataPipe,rxAddr,5);
	}else{
		nRF24_writeRegister(RX_ADDR_P0+dataPipe,rxAddr,1);
	}
	
	return *rxAddr;
}

/*! Interrupt settings.
* \note The IRQ pin must be connected to the MCU and the module should not be in any transmission mode during IRQ configuration.
* \par The interrupt flags in \c STATUS register should be cleared before enabling IRQ's.
* \param irqVal - pass the combination of interrupt mnemonics (TX_DS, RX_DR, MAX_RT) which will be enabled/disabled. You may enable/disable more than one IRQ at a time by summing mnemonics with the '|' operator;
* \param irqEn - \c '1': enable IRQ's with \c irqVal, \c '0': disable IRQ's with \c irqVal;
* \return \c CONFIG register value;
* \sa 
*/
uint8_t nRF24_enDisIRQ(uint8_t irqVal, _Bool irqEn){
	uint8_t configReg;
	
	nRF24_readRegister(CONFIG,&configReg,1);
	if(irqEn){
		configReg &= ~irqVal; //'0' - IRQ reflected on the pin
	}else{
		configReg |= irqVal;
	}
  nRF24_writeRegister(CONFIG,&configReg,1);
   
	return configReg;
}

/*! CRC settings.
* \note The module should not be in any transmission mode during CRC configuration.
* \param crc - \c '0': no CRC, \c '1': 1 byte CRC, \c default: 2 byte CRC;
* \return \c CONFIG register value;
* \sa 
*/
uint8_t nRF24_setCRC(uint8_t crc){
	uint8_t configReg;
	
	nRF24_readRegister(CONFIG,&configReg,1);
	switch(crc){
		case 0:
			configReg &= ~(EN_CRC | CRC0);
			break;
		case 1:
			configReg |= EN_CRC;
			configReg &= ~CRC0;
			break;
		default:
			configReg |= (EN_CRC | CRC0);
	}
  nRF24_writeRegister(CONFIG,&configReg,1);
  
	return configReg;
}

/*! Auto ACK settings.
* \note The module should not be in any transmission mode during Auto ACK configuration.
* \param AAval - pass the combination of EN_AA mnemonics (ENAA_P0-5) which will be enabled/disabled. You may enable/disable Auto ACK on more than one data pipe at a time by summing mnemonics with the '|' operator;
* \param AAen - \c '1': enable Auto ACK with \c AAval, \c '0': disable Auto ACK with \c AAval;
* \return \c EN_AA register value;
* \sa nRF24_enDisDataPipe(),nRF24_enDisDynPayLen(),nRF24_enDisDynACK(),nRF24_enDisACKpayload()
*/
uint8_t nRF24_enDisAutoACK(uint8_t AAval, _Bool AAen){
	uint8_t en_aaReg;
	
	nRF24_readRegister(EN_AA,&en_aaReg,1);
	if(AAen){
		en_aaReg |= AAval;
	}else{
		en_aaReg &= ~AAval;
	}
  nRF24_writeRegister(EN_AA,&en_aaReg,1);
  
	return en_aaReg;
}
	
/*! Data pipes settings.
* \note The module should not be in any transmission mode during data pipe configuration.
* \param dataPipeVal - pass the combination of EN_RXADDR mnemonics (ERX_P0-5) which will be enabled/disabled. You may enable/disable more than one data pipe at a time by summing mnemonics with the '|' operator;
* \param dataPipeEn - \c '1': enable data pipes with \c dataPipeVal, \c '0': disable data pipes with \c dataPipeVal;
* \return \c EN_RXADDR register value;
* \sa nRF24_enDisAutoACK(),nRF24_enDisDynPayLen(),nRF24_enDisDynACK(),nRF24_enDisACKpayload()
*/
uint8_t nRF24_enDisDataPipe(uint8_t dataPipeVal, _Bool dataPipeEn){
	uint8_t en_rxaddrReg;
	
	nRF24_readRegister(EN_RXADDR,&en_rxaddrReg,1);
	if(dataPipeEn){
		en_rxaddrReg |= dataPipeVal;
	}else{
		en_rxaddrReg &= ~dataPipeVal;
	}
  nRF24_writeRegister(EN_RXADDR,&en_rxaddrReg,1);
  
	return en_rxaddrReg;
}

/*! Set Address Width for data pipes.
* \note The module should not be in any transmission mode during Address Width configuration.
* \param addrW - \c '5': 5 bytes, \c '4': 4 bytes, \c '3': 3 bytes, \c default: 5 bytes;
* \return \c SETUP_AW register value;
* \sa 
*/
uint8_t nRF24_setAddresWidth(uint8_t addrW){
	uint8_t setup_awReg;

	if(addrW<3 || addrW>5){
		addrW=5;
	}
	setup_awReg=AW(addrW-2);
  nRF24_writeRegister(SETUP_AW,&setup_awReg,1);
  
	return setup_awReg;
}

/*! Set Automatic Retransmission Delay.
* \note The module should not be in any transmission mode during Automatic Retransmission Delay configuration.
* \param ard - \c '15': 4000 us, \c '14': 3750 us, ... , \c '1': 500 us, \c '0': 250 us, \c default: 4000 us;
* \return \c SETUP_RETR register value;
* \sa nRF24_setAutoRetranCount()
*/
uint8_t nRF24_setAutoRetranDelay(uint8_t ard){
	uint8_t setup_retrReg;

	if(ard>15){
		ard=15;
	}
	nRF24_readRegister(SETUP_RETR,&setup_retrReg,1);
	setup_retrReg |= ARD(ard);
  nRF24_writeRegister(SETUP_RETR,&setup_retrReg,1);

	return setup_retrReg;
}

/*! Set Automatic Retransmission Count.
* \param ard - \c '15': up to 15 times, \c '14': up to 14 times, ... , \c '1': up to 1 time, \c '0': no auto retransmissions, \c default: up to 15 times;
* \return \c SETUP_RETR register value;
* \sa nRF24_setAutoRetranDelay()
*/
uint8_t nRF24_setAutoRetranCount(uint8_t arc){
	uint8_t setup_retrReg;

	if(arc>15){
		arc=15;
	}
	nRF24_readRegister(SETUP_RETR,&setup_retrReg,1);
	setup_retrReg |= ARC(arc);
  nRF24_writeRegister(SETUP_RETR,&setup_retrReg,1);

	return setup_retrReg;	
}

/*! Set RF Channel.
* \param channel - number of RF Channel on which the module will operate;
* \return \c RF_CH register value;
* \sa nRF24_setRFdataRate(),nRF24_setRFoutputPower()
*/
uint8_t nRF24_setRFchannel(uint8_t channel){
	uint8_t rf_chReg;

	rf_chReg=RF_CH_MASK(channel);
  nRF24_writeRegister(RF_CH,&rf_chReg,1);
  
	return rf_chReg;
}

/*! Set RF Data Rate.
* \param dataRate - \c '1': 1 Mbps, \c '0': 250 Kbps, \c default: 2 Mbps;
* \return \c RF_SETUP register value;
* \sa nRF24_setRFchannel(),nRF24_setRFoutputPower()
*/
uint8_t nRF24_setRFdataRate(uint8_t dataRate){
	uint8_t rf_setupReg;

	nRF24_readRegister(RF_SETUP,&rf_setupReg,1);
	switch(dataRate){
		case 0:
			rf_setupReg |= RF_DR_LOW;
			rf_setupReg &= ~RF_DR_HIGH;
			break;
		case 1:
			rf_setupReg &= ~RF_DR_LOW;
			rf_setupReg &= ~RF_DR_HIGH;
			break;
		default:
			rf_setupReg &= ~RF_DR_LOW;
			rf_setupReg |= RF_DR_HIGH;
	}
  nRF24_writeRegister(RF_SETUP,&rf_setupReg,1);
  
	return rf_setupReg;	
}

/*! Set RF Output Power.
* \param power - \c '2': -6 dBm, \c '1': -12 dBm, \c '0': -18 dBm, \c default: 0 dBm;
* \return \c RF_SETUP register value;
* \sa nRF24_setRFdataRate(),nRF24_setRFchannel()
*/
uint8_t nRF24_setRFoutputPower(uint8_t power){
	uint8_t rf_setupReg;

	nRF24_readRegister(RF_SETUP, &rf_setupReg, 1);
	rf_setupReg &= ~RF_PWR(3); //clear actual power settings
	switch(power){
		case 0:
			rf_setupReg |= RF_PWR(0);
			break;
		case 1:
			rf_setupReg |= RF_PWR(1);
			break;
		case 2:
			rf_setupReg |= RF_PWR(2);
			break;
		default:
			rf_setupReg |= RF_PWR(3);
	}
	nRF24_writeRegister(RF_SETUP, &rf_setupReg, 1);

	return rf_setupReg;
}

/*! Start PLL carrier test.
* \detail Not implemented yet.
*/
uint8_t nRF24_PLLcarrierTest(void){
	//not implemented yet
	return 0;
}

/*! Get Packet Loss Counter.
* \return Packet Loss Counter value;
* \sa nRF24_getPacketRetranCount()
*/
uint8_t nRF24_getPacketLossCount(void){
	uint8_t observe_txReg;

	nRF24_readRegister(OBSERVE_TX, &observe_txReg, 1);

	return (observe_txReg>>4);
}

/*! Get Packet Retransmit Counter.
* \return Packet Retransmit Counter value;
* \sa nRF24_getPacketLossCount()
*/
uint8_t nRF24_getPacketRetranCount(void){
	uint8_t observe_txReg;

	nRF24_readRegister(OBSERVE_TX, &observe_txReg, 1);

	return (observe_txReg & 0x0F);
}

/*! Set payload width for specific data pipe.
* \param dataPipe - number of data pipe the payload width will be set (0-5);
* \param payWidth - amount of bytes in the payload for given data pipe (0-32);
* \return if payWidth and dataPipe values are in correct value range, the function returns payload width value. Otherwise, the \c '0xFF' is returned;
* \sa nRF24_getRXpayWidth()
*/
uint8_t nRF24_setRXpayloadWidth(uint8_t dataPipe, uint8_t payWidth){		
	if(dataPipe>5 || payWidth > 32){
		return 0xFF; //error avoidance
	}

	nRF24_writeRegister(RX_ADDR_P0 + dataPipe, &payWidth, 1);

	return payWidth;
}

/*! Dynamic Payload Length settings.
* \note The module should not be in any transmission mode during Dynamic Payload Length configuration.
* \warning In order to enable DPL on data pipes, Auto ACK must be enabled for them (EN_AA configuration).
* \param dataPipeVal - pass the combination of DYN_PD mnemonics (DPL_P0-5) which will be enabled/disabled. You may enable/disable DPL on more than one data pipe at a time by summing mnemonics with the '|' operator;
* \param DPLenable - \c '1': enable DPL on data pipes with \c dataPipeVal, \c '0': disable DPL on data pipes with \c dataPipeVal;
* \return \c DYN_PD register value;
* \sa nRF24_enDisDataPipe(),nRF24_enDisAutoACK(),nRF24_enDisDynACK(),nRF24_enDisACKpayload()
*/
uint8_t nRF24_enDisDynPayLen(uint8_t dataPipeVal, _Bool DPLenable){
	uint8_t dyn_pdReg,featureReg;
		
	nRF24_readRegister(DYN_PD, &dyn_pdReg, 1);
	nRF24_readRegister(FEATURE, &featureReg, 1);
	if(DPLenable){
		dyn_pdReg |= dataPipeVal;
		featureReg |= EN_DPL;
	}else{
		dyn_pdReg &= ~dataPipeVal;
		if(dyn_pdReg==0){
			featureReg &= ~EN_DPL;
		}
	}
	nRF24_writeRegister(DYN_PD, &dyn_pdReg, 1);

	return dyn_pdReg;
}

/*! ACK Payload settings.
* \note The module should not be in any transmission mode during ACK Payload configuration.
* \warning If ACK packet payload is activated, ACK packets have dynamic payload length and Dynamic Payload Length feature should be enabled for data pipe 0 on the PTX and PRX devices (EN_DPL,ENAA_P0,DPL_P0). This is to ensure that they receive ACK packets with payloads. Also set the correct ARD value..
* \param enDis - \c '1': enable ACK Payload, \c '0': disable ACK Payload;
* \return \c FEATURE register value;
* \sa nRF24_enDisDataPipe(),nRF24_enDisDynPayLen(),nRF24_enDisDynACK(),nRF24_enDisAutoACK()
*/
uint8_t nRF24_enDisACKpayload(_Bool enDis){
	uint8_t featureReg;
		
	nRF24_readRegister(FEATURE, &featureReg, 1);
	if(enDis){
		featureReg |= EN_ACK_PAY;
	}else{
		featureReg &= ~ EN_ACK_PAY;
	}
	nRF24_writeRegister(FEATURE, &featureReg, 1);

	return featureReg;
}

/*! Dynamic ACK settings.
* \note The module should not be in any transmission mode during Dynamic ACK configuration.
* \param enDis - \c '1': enable Dynamic ACK, \c '0': disable Dynamic ACK;
* \return \c FEATURE register value;
* \sa nRF24_enDisDataPipe(),nRF24_enDisDynPayLen(),nRF24_enDisAutoACK(),nRF24_enDisACKpayload()
*/
uint8_t nRF24_enDisDynACK(_Bool enDis){
	uint8_t featureReg;
		
	nRF24_readRegister(FEATURE, &featureReg, 1);
	if(enDis){
		featureReg |= EN_DYN_ACK;
	}else{
		featureReg &= ~ EN_DYN_ACK;
	}
	nRF24_writeRegister(FEATURE, &featureReg, 1);

	return featureReg;
}

/*! Set TX Payload Reuse.
* \note The module should not be in any transmission mode during TX Payload Reuse configuration.
* \warning TX Payload Reuse may be set only if there was at least one packet successfully transmitted after powering up the module. 
* \param payloadReuse - \c '1': enable TX Payload Reuse, \c '0': disable TX Payload Reuse and flush TX FIFO;
* \return \c FIFO_STATUS register value;
* \sa nRF24_flushTX()
*/
uint8_t nRF24_setTXpayloadReuse(_Bool payloadReuse){
	uint8_t fifo_statusReg;

	if(payloadReuse){
		nRF24_sendCommand(REUSE_TX_PL);		
	}else{
		nRF24_sendCommand(FLUSH_TX);	
	}
	nRF24_readRegister(FIFO_STATUS, &fifo_statusReg, 1);

	return fifo_statusReg;
}

/*! Flush RX FIFO.
* \return \c FIFO_STATUS register value;
* \sa nRF24_flushTX()
*/
uint8_t nRF24_flushRX(void){
	uint8_t fifo_statusReg;

	nRF24_sendCommand(FLUSH_RX);		
	nRF24_readRegister(FIFO_STATUS, &fifo_statusReg, 1);

	return fifo_statusReg;
}

/*! Flush TX FIFO.
* \return \c FIFO_STATUS register value;
* \sa nRF24_flushRX()
*/
uint8_t nRF24_flushTX(void){
	uint8_t fifo_statusReg;

	nRF24_sendCommand(FLUSH_TX);		
	nRF24_readRegister(FIFO_STATUS, &fifo_statusReg, 1);

	return fifo_statusReg;
}

/*! Get payload width for the next RX Payload in RX FIFO.
* \return payload width for the next RX Payload in RX FIFO;
* \sa nRF24_setRXpayloadWidth()
*/
uint8_t nRF24_getRXpayWidth(void){		
	nRF24_sendCommand(R_RX_PL_WID);		

	return spi1_read_byte(NOP);
}

/*! Send data to TX FIFO.
* \note The module should not be in any transmission mode during writing to TX FIFO.
* \param val - a pointer to data array;
* \param len - amount of bytes to send (0-32). If the \c len value is greater than 32, the function will write only first 32 bytes to TX FIFO;
* \return \c FIFO_STATUS register value;
* \sa nRF24_sendDataNOACK(),nRF24_receiveData()
*/
uint8_t nRF24_sendData(uint8_t* data, uint8_t len){
	uint8_t fifo_statusReg,i;

	do{
		fifo_statusReg=nRF24_getFIFOstatus();
	}while(fifo_statusReg & TX_FULL_FS);

	if(len>32){
		len=32;
	}
	pin_CSN(LOW);
	delay_ms(5);
  spi1_byte_send(W_TX_PAYLOAD,0);
	delay_us(10);
	for(i=0; i<len; i++){
		spi1_byte_send(*(data+i) , 0);
  }
	pin_CSN(HIGH);
	delay_us(10);
	
	fifo_statusReg=nRF24_getFIFOstatus();
	return fifo_statusReg;
}

/*! Send data to TX FIFO with NOACK flag set.
* \note The module should not be in any transmission mode during writing to TX FIFO.
* \par In order to use this function, the Dynamic ACK must be enabled (FEATURE).
* \param val - a pointer to data array;
* \param len - amount of bytes to send (0-32). If the \c len value is greater than 32, the function will write only first 32 bytes to TX FIFO;
* \return \c FIFO_STATUS register value;
* \sa nRF24_sendData(),nRF24_receiveData(),nRF24_enDisDynACK() 
*/
uint8_t nRF24_sendDataNOACK(uint8_t* data, uint8_t len){
	uint8_t fifo_statusReg,i;

	do{
		fifo_statusReg=nRF24_getFIFOstatus();
	}while(fifo_statusReg & TX_FULL_FS);

	if(len>32){
		len=32;
	}
	pin_CSN(LOW);
	delay_ms(5);
  spi1_byte_send(W_TX_PAYLOAD_NOACK,0);
	delay_us(10);
	for(i=0; i<len; i++){
		spi1_byte_send(*(data+i) , 0);
	}
	pin_CSN(HIGH);
	delay_us(10);

	fifo_statusReg=nRF24_getFIFOstatus();
	return fifo_statusReg;
}

/*! Receive data from RX FIFO.
* \note Usage of nRF24_getRXpayWidth() function to specify the \c len value is recommended.
* \param dest - a pointer to the destination array, where the received data will be stored;
* \param len - amount of bytes to send (0-32). If the \c len value is greater than 32, the function will write only first 32 bytes to TX FIFO;
* \return if there is less bytes in RX FIFO then the \c len value, the function returns \c '0xFF'. Otherwise, \c FIFO_STATUS register value is returned;
* \sa nRF24_sendData(),nRF24_sendDataNOACK(),nRF24_getRXpayWidth()
*/
uint8_t nRF24_receiveData(uint8_t* dest, uint8_t len){
	uint8_t fifo_statusReg,i;

	pin_CSN(LOW);
	delay_ms(5);
  spi1_byte_send(R_RX_PAYLOAD,0);
	delay_us(10);
	for(i=0; i<len; i++){
		*(dest+i)=spi1_read_byte(NOP);
		if(nRF24_getFIFOstatus() & RX_EMPTY){
			return 0xFF; //error avoidance
		}
	}
	pin_CSN(HIGH);
	delay_us(10);

	fifo_statusReg=nRF24_getFIFOstatus();
	return fifo_statusReg;
}
//!@}
