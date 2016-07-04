/*----------------------------------------------------------------------------
 *      
 *----------------------------------------------------------------------------
 *      Name:    spi.h
 *      Purpose: SPI settings
 *----------------------------------------------------------------------------
 *      
 *---------------------------------------------------------------------------*/
#ifndef SPI_H
	#define SPI_H
	 
	#include "MKL46Z4.h" 
	
	void spi0init(void);
	void spi1init(void);
	int spi1_byte_send(uint8_t data, _Bool receive);
	void spi0_byte_send(uint8_t data);
	void spi1_data_send(uint8_t *data , int size, _Bool receive);
	void spi0_set_match_value_ML(uint8_t ML);
	void spi0_set_match_value(uint8_t ML,uint8_t MH);
	void spi1_set_receive_buffer(uint8_t *receive_buffer, int receive_buffer_size);
	uint8_t spi1_read_byte(uint8_t data);

	extern uint8_t *spi1_data_s_pointer;
	extern int spi1_data_s_size;
	extern int count;
	extern _Bool spi1_busy;
	extern uint8_t *spi1_receive_buffer;
	extern int spi1_receive_buffer_size;
	extern int spi1_receive_buffer_counter;

	/* Pins using as SPI0 I/O*/ 
	#define SPI0_MISO_PORT E
	#define SPI0_MISO 19
	#define SPI0_MISO_MUX 2
	#define SPI0_MOSI_PORT E
	#define SPI0_MOSI 18
	#define SPI0_MOSI_MUX 2
	#define SPI0_SCK_PORT E
	#define SPI0_SCK 17
	#define SPI0_SCK_MUX 2
	#define SPI0_PCS0_PORT E
	#define SPI0_PCS0 16
	#define SPI0_PCS0_MUX 2

	/* Pins using as SPI1 I/O*/ 
	#define SPI1_MISO_PORT E
	#define SPI1_MISO 0
	#define SPI1_MISO_MUX 2
	#define SPI1_MOSI_PORT E
	#define SPI1_MOSI 1
	#define SPI1_MOSI_MUX 2
	#define SPI1_SCK_PORT E
	#define SPI1_SCK 2
	#define SPI1_SCK_MUX 2
	#define SPI1_PCS1_PORT E
	#define SPI1_PCS1 3
	#define SPI1_PCS1_MUX 2



	/*Baud rate set for SPI0
	The input to this prescaler is
	the bus rate clock (BUSCLK). The output of this prescaler drives the input of the SPI baud rate divider.
	Refer to the description of “SPI Baud Rate Generation” for details.
	0 Baud rate prescaler divisor is 1.
	1 Baud rate prescaler divisor is 2.
	2 Baud rate prescaler divisor is 3.
	3 Baud rate prescaler divisor is 4.
	4 Baud rate prescaler divisor is 5.
	5 Baud rate prescaler divisor is 6.
	6 Baud rate prescaler divisor is 7.
	7 Baud rate prescaler divisor is 8.
	*/
	#define SPI0_SPPR 7
	#define SPI1_SPPR 7


	/*This 4-bit field selects one of nine divisors for the SPI baud rate divider. The input to this divider comes
	from the SPI baud rate prescaler. Refer to the description of “SPI Baud Rate Generation” for details.
	0 Baud rate divisor is 2.
	1 Baud rate divisor is 4.
	2 Baud rate divisor is 8.
	3 Baud rate divisor is 16.
	4 Baud rate divisor is 32.
	5 Baud rate divisor is 64.
	6 Baud rate divisor is 128.
	7 Baud rate divisor is 256.
	8 Baud rate divisor is 512.
	All others Reserved*/
	#define SPI0_SPR 4
	#define SPI1_SPR 8


	/*SPI0 transfer mode 0 - 8 bit word 1- 16 bit word*/ 
	#define SPI0_DATA_LENGTH 0
	#define SPI1_DATA_LENGTH 0


	/*SPI0 as Master - 1 as Slave - 0 */
	#define SPI0_MS_MODE 1
	#define SPI1_MS_MODE 1


	/*SPI0 clock polarity 0 - Active-high clock polarity 1-Active-low polarity*/
	#define SPI0_CLOCK_POLARITY 0
	#define SPI1_CLOCK_POLARITY 0


	/*SPI0 clock phase 
	*
	*1-First edge on SPSCK occurs at the middle of the first cycle of a data transfer.
	*0-First edge on SPSCK occurs at the start of the first cycle of a data transfer.
	*
	*/
	#define SPI0_CLOCK_PHASE 1
	#define SPI1_CLOCK_PHASE 0


	/*SPI0 first send MSB - 0 LSB -1*/
	#define SPI0_FIRST_BIT 0
	#define SPI1_FIRST_BIT 0


	/*SPI0 transfer mode 1 - bidairectional 0 - one directional*/
	#define SPI0_TRANSFER_MODE 0
	#define SPI1_TRANSFER_MODE 0


	/*SPI0 data memory transfer 1 - DMA transfer 0 - no DMA transfer*/
	#define SPI0_DMA_MODE 0
	#define SPI1_DMA_MODE 0

	/*SPI1 1-FIFO mode 0-no FIFO mode*/ 
	#define SPI1_FIFO_MODE 1








	// Makra upraszczajace dostep do portów
	// *** PORT
	#define PORT(x) SPORT(x)
	#define SPORT(x) (PORT##x)
	//Choosing SPI between 0 and 1
	#define SPI(x) SSPI(x)
	#define SSPI(x) (SPI##x)

#endif
