#include "lang4robots.h"
#include "slcd.h"
#include "SPI.h"
#include "delay.h"
#include "pinManagement.h"
#include "nRF24.h"

#define MASTER	0

int main (void)
{
	uint8_t comm=0;
	
	slcdInitialize();
	pin_Init();
	spi1init();
	pin_CE(LOW);
	nRF24_init();
	
	if(MASTER){
		while(1){
			/*
			pin_CE(LOW);
			delay_us(10);
			nRF24_sendData(&comm, 1);
			nRF24_modeTX();
			pin_CE(HIGH);
			delay_ms(2000);*/
			lang4robots_sendCommand(_TX_ADDR,comm);
			delay_ms(2000);
			comm++;
			if(comm>2){
				comm=0;
			}
		}
	}else{
		//delay_ms(10);
		nRF24_modeRX();
		pin_CE(HIGH);
	}
	while(1){
		
	}

}

//IRQ Handler for module IRQ pin - move this function to main.c file//
void PORTC_PORTD_IRQHandler(void){ //check irqhandler name
  if(PIN_IRQ_SOURCE(PORT_IRQ, PIN_IRQ)){
    uint8_t statusReg,irq_mask;
    PIN_IRQ_CLEAR_FLAG(PORT_IRQ, PIN_IRQ);
		pin_CE(LOW);
    statusReg=nRF24_getStatus();
		delay_us(10);
		
    if(statusReg & RX_DR){
      //data received routine
			irq_mask|=RX_DR;
			nRF24_writeRegister(STATUS,&irq_mask,1);
			//nRF24_receiveData(&irq_mask,1);
			irq_mask=((statusReg&RX_P_NO(7))>>1);
			if(irq_mask>5){
				slcdErr(6);
			}else{
				//slcdDisplay((uint16_t)lang4robots_receiveCommand(irq_mask),16);
				lang4robots_executeCommand(lang4robots_receiveCommand(irq_mask),0);
			}
			pin_CSN(HIGH);
			//while(1){;}
    }if(statusReg & TX_DS){
      //data sent routine
			irq_mask|=TX_DS;
			nRF24_writeRegister(STATUS,&irq_mask,1);
			pin_CSN(HIGH);
			//while(1){;}
    }if(statusReg & MAX_RT){
      //max retransmission routine
			irq_mask|=MAX_RT;
			nRF24_writeRegister(STATUS,&irq_mask,1);
			slcdDisplay((uint16_t)nRF24_getPacketLossCount(),16);
			pin_CSN(HIGH);
			delay_ms(1000);
			//while(1){;}
    }
		
		//delay_ms(10);
		pin_CE(HIGH);
  }else{
    //error
  }
}
