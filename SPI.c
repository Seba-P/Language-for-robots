#include "spi.h"
#include "slcd.h"

uint8_t *spi1_data_s_pointer;
int spi1_data_s_size;
int count;
_Bool spi1_busy;
uint8_t *spi1_receive_buffer;
int spi1_receive_buffer_size;
int spi1_receive_buffer_counter;
int spi1_recive_mode;
uint8_t data;





void spi1init()
{
	int buffer= 0 ;
	SIM -> SCGC4 |= SIM_SCGC4_SPI0_MASK;  														 //clock on in spi 
	SIM -> SCGC5 |= SIM_SCGC5_PORTE_MASK;
	
	SPI0 -> BR |= SPI_BR_SPPR(SPI0_SPPR);
	SPI0 -> BR |= SPI_BR_SPR(SPI0_SPR);
	PORT(SPI0_MISO_PORT) -> PCR[SPI0_MISO] |= PORT_PCR_MUX(SPI0_MISO_MUX);
	PORT(SPI0_MOSI_PORT) -> PCR[SPI0_MOSI] |= PORT_PCR_MUX(SPI0_MOSI_MUX);
  PORT(SPI0_PCS0_PORT) -> PCR[SPI0_PCS0] |= PORT_PCR_MUX(SPI0_PCS0_MUX);
	PORT(SPI0_SCK_PORT)  ->  PCR[SPI0_SCK] |= PORT_PCR_MUX(SPI0_SCK_MUX);

	SPI0 -> C1 |= (SPI0_MS_MODE << SPI_C1_MSTR_SHIFT);								  //Set mode master/slave
	SPI0 -> C1 |= (SPI0_FIRST_BIT << SPI_C1_LSBFE_SHIFT);   					  //First bit LSB/MSB
	SPI0 -> C1 &= ~(SPI0_CLOCK_PHASE << SPI_C1_CPHA_SHIFT);  					  //Clock phase 
	SPI0 -> C1 |= (SPI0_CLOCK_POLARITY << SPI_C1_CPOL_SHIFT); 				  //Clock polarity
	
	SPI0 -> C2 |= (SPI0_DATA_LENGTH << SPI_C2_SPIMODE_SHIFT);						//seting mode 8/16 bit
	SPI0 -> C2 |= (SPI0_DMA_MODE << SPI_C2_TXDMAE_SHIFT);								//DMA mode 
	SPI0 -> C2 |= (SPI0_TRANSFER_MODE << SPI_C2_BIDIROE_SHIFT);					//one/two directional mode 
	
	//SPI0 -> C1 |= SPI_C1_SPIE_MASK;
	//SPI0 -> C1 |= SPI_C1_SPTIE_MASK;
	//NVIC_ClearPendingIRQ(SPI0_IRQn);																		// Clear NVIC any pending interrupts on SPI1 
	//NVIC_EnableIRQ(SPI0_IRQn);
	
	
	SPI0 -> C1 |= SPI_C1_SPE_MASK; 																			//On SPI;
																		
}

int spi1_byte_send(uint8_t data, _Bool receive)        //zmienione
{
	uint8_t reg;
	SPI0->C1 |= SPI_C1_SPIE_MASK;
	while(!(SPI0->S &(1<<SPI_S_SPTEF_SHIFT)));
	SPI0->DL = data;
	//return 0;
	
	//SPI0 ->S &(1<<SPI_S_SPRF_SHIFT);
	while( 0==(SPI0 ->S &(1<<SPI_S_SPRF_SHIFT)))
	{};
	return SPI0->DL;
}

uint8_t spi1_read_byte(uint8_t data)
{
	uint8_t retu;
	uint8_t reg;
	
	SPI0->C1 &= ~(1<<SPI_C1_SPIE_SHIFT);
	while(!(SPI0->S &(1<<SPI_S_SPTEF_SHIFT)));
	SPI0->DL = data;
	//reg = SPI0 ->S &(1<<SPI_S_SPRF_SHIFT);
	while( 0==(SPI0 ->S &(1<<SPI_S_SPRF_SHIFT)))
	{};
	//{
	//SPI0 ->S &(1<<SPI_S_SPRF_SHIFT);
	//};
			//retu = ;
		return SPI0->DL;
}

void spi0_set_match_value(uint8_t ML, uint8_t MH)
{
	SPI0 -> ML = ML;
	SPI0 -> MH = MH;
}

void spi0_set_match_value_ML(uint8_t ML)
{
	SPI0 -> ML = ML;
}


	
	
	/*void SPI0_IRQHandler(void)
	{
		if(SPI0-> S & (1<<SPI_S_SPTEF_SHIFT))
			SPI0->DL = 0xAA;
		
		if(SPI0-> S & (1<<SPI_S_SPRF_SHIFT))
		{
		slcdClear();
		slcdDisplay((uint16_t) SPI0 -> DL ,10);	
		}
		
	}*/
	

