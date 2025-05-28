#include "i2c_dma.h"

/*______________________________________
 *______________________________________
 *
 *RCC APB1 peripheral clock enable register (RCC_APB1ENR)
 *Bit 21 I2C1EN: I2C1 clock enable
 *______________________________________
 *______________________________________
 *
 *Go to Alternate function mapping in DATASHEET
 *AF4
 *PB6 is I2C1_SCL
 *PB7 is I2C1_SDA
 *______________________________________
 *______________________________________
 *
 *Go to DMA1 request mapping in REF MANUAL
 *USE Channel 1
 *USE Stream 5 and Stream 6 for I2C1_RX and I2C1_TX respectively
 *______________________________________
 *______________________________________
 */
#define I2C1_EN	            (1U<<21)
#define I2C1_PE	            (1U<<0)
#define TCIF5               (1U<<11)
#define TCIF6               (1U<<21)

#define I2C1_SWRST           (1U<<15)
#define I2C1_NOSTRETCH       (1U<<7)
#define I2C1_ENGC            (1U<<6)
#define DMA_S_CR_EN          (1U<<0)
#define DMA_S_CR_TCIE		 (1U<<4)
#define DMA_S_CR_MINC        (1U<<10)
#define I2C1_DMAEN           (1U<<11)
#define I2C1_LAST            (1U<<12)
#define I2C1_SR2_BUSY        (1U << 1)
#define I2C1_START	         (1U << 8)
#define I2C1_STOP            (1U<<9)
#define I2C1_SB	       		  (1U << 0)
#define I2C1_ADDR	       	  (1U << 1)
#define I2C1_TXE	       	  (1U << 7)
#define I2C1_ACK	       	 (1U << 10)
#define I2C1_BTF	       	 (1U << 2)

void i2c1_init(void){

	//*************************************
	//I2C1 Peripheral Configuration Steps
	//*************************************

	//--------------------------------------
    //-----------gpiob config--------------
	//--------------------------------------

	// enable clock access for GPIOB (pb6 and pb7)
	RCC->AHB1ENR |= GPIOBEN;

	// enable clock access for i2c1
	RCC->APB1ENR |= I2C1_EN;

	// set mode as AF for pb6 and pb7 (10: Alternate function mode)
	GPIOB->MODER &= ~(1U<<12);
	GPIOB->MODER |= (1U<<13); //pb6

	GPIOB->MODER &= ~(1U<<14);
	GPIOB->MODER |= (1U<<15); //pb7

	// set PB6 and PB7 alternate function type to I2C1 (0100: AF4)
	GPIOB->AFR[0] &= ~(1U<<24);
	GPIOB->AFR[0] &= ~(1U<<25);
	GPIOB->AFR[0] |= (1U<<26);
	GPIOB->AFR[0] &= ~(1U<<27); //pb6

	GPIOB->AFR[0] &= ~(1U<<28);
	GPIOB->AFR[0] &= ~(1U<<29);
	GPIOB->AFR[0] |= (1U<<30);
	GPIOB->AFR[0] &= ~(1U<<31); //pb7

	// set output type as open-drain outputs with pull-up
	GPIOB->OTYPER |= ((1U << 6) | (1U << 7));
	//GPIOB->PUPDR |= (1U << 12) | (1U << 14);

	//--------------------------------------
	//-----------i2c1 config--------------
	//--------------------------------------

	// _6_ Configure I2C control register

	I2C1->CR1 |= I2C1_SWRST; //reset module
	I2C1->CR1 &= ~I2C1_SWRST; //release

	I2C1->CR1 &= ~I2C1_NOSTRETCH; // wait for slave

	I2C1->CR1 &= ~I2C1_ENGC; //General call disabled

	I2C1->CR2 |= I2C1_DMAEN; //DMA request enabled

	I2C1->CR2 |= I2C1_LAST; //DMA EOT is the last transfer

	I2C1->CR2 = 16; // Peripheral clock frequency 16MHz

	I2C1->CCR = 80; //Set standard mode speed to 100kHz

	/*
	 * 		TRISE = (1000ns / T_PCLK) + 1
			T_PCLK = 1 / Fpclk = 1 / 16MHz = 62.5ns
			TRISE = 1000 / 62.5 + 1 = 16 + 1 = 17
	 *
	 */
	I2C1->TRISE = 17;
	// _7_ Enable I2C1 peripheral (set PE bit in CR1)
	I2C1->CR1 |= I2C1_PE; // PE bit → I2C enable

	//--------------------------------------
}



void i2c_tx_dma_stream6(void){

	//DMA1 clock enabled
	RCC->AHB1ENR |=DMA1EN;

	//disable dma
	DMA1_Stream6->CR = 0;
	while(DMA1_Stream6->CR & DMA_S_CR_EN){}

	//Channel selection: channel 1: 001
	DMA1_Stream6->CR |= (1U<<25);
	DMA1_Stream6->CR &= ~(1U<<26);
	DMA1_Stream6->CR &= ~(1U<<27);

	//Memory increment is enabled
	DMA1_Stream6->CR |= DMA_S_CR_MINC;

	//Transfer complete interrupt is enabled
	DMA1_Stream6->CR |= DMA_S_CR_TCIE;

	//Transfer dir: 01: Memory-to-peripheral
	DMA1_Stream6->CR |= (1U<<6);
	DMA1_Stream6->CR &= ~(1U<<7);

	//Enable stream interrupt in nvic
	NVIC_EnableIRQ(DMA1_Stream6_IRQn);

}

void i2c_rx_dma_stream5(void){
	//DMA1 clock enabled
	RCC->AHB1ENR |=DMA1EN;

	//disable dma
	DMA1_Stream5->CR = 0;
	while(DMA1_Stream5->CR & DMA_S_CR_EN){}

	//Channel selection: channel 1: 001
	DMA1_Stream5->CR |= (1U<<25);
	DMA1_Stream5->CR &= ~(1U<<26);
	DMA1_Stream5->CR &= ~(1U<<27);

	//Memory increment is enabled
	DMA1_Stream5->CR |= DMA_S_CR_MINC;

	//Transfer complete interrupt is enabled
	DMA1_Stream5->CR |= DMA_S_CR_TCIE;

	//Transfer dir: 01: peripheral to Memory
	DMA1_Stream5->CR &= ~(1U<<6);
	DMA1_Stream5->CR &= ~(1U<<7);

	//Enable stream interrupt in nvic
	NVIC_EnableIRQ(DMA1_Stream5_IRQn);

}

void i2c_dma_transfer(uint8_t *data, uint32_t size) {
	//stream 6
	if(NULL != data){

		// clear transfer complete interrupt flag
		DMA1->HIFCR |= TCIF6; //CTCIF6

		//peripheral address
		DMA1_Stream6->PAR = (uint32_t)(&(I2C1->DR));

		//memory address
		DMA1_Stream6->M0AR = (uint32_t)data;

		//transfer length
		DMA1_Stream6->NDTR = size;

		DMA1_Stream6->CR |= DMA_S_CR_EN; // EN

	}


}

void i2c_dma_receive(uint8_t *buffer, uint32_t size) {
	//stream 5
	if(NULL != buffer){
	// clear transfer complete interrupt flag
	DMA1->HIFCR |= TCIF5; //CTCIF5

	//peripheral address
	DMA1_Stream5->PAR = (uint32_t)(&(I2C1->DR));

	//memory address
	DMA1_Stream5->M0AR = (uint32_t)buffer;

	//transfer length
	DMA1_Stream5->NDTR = size;

	DMA1_Stream5->CR |= DMA_S_CR_EN; // EN

	}

}

void i2c_read(uint8_t slave_address, uint8_t reg, uint8_t *data, uint32_t size){

	 // Wait for bus is not busy
	while ((I2C1->SR2 & I2C1_SR2_BUSY)){}

    I2C1->CR1 |= I2C1_START; // START

    while (!(I2C1->SR1 & I2C1_SB)){} // SB

    (void)I2C1->SR1;
    //Cleared by reading the SR1 register

    I2C1->DR = (slave_address << 1); // Write

    while (!(I2C1->SR1 & I2C1_ADDR)){} // ADDR
    //Received address matched

    (void)I2C1->SR1;
    (void)I2C1->SR2;
    //clearing sequence (READ SR1 then READ SR2) after ADDR is set.

    while (!(I2C1->SR1 & I2C1_TXE)){} // TXE
    //Data register is empty (transmitters)

    if(size >= 2){
    	I2C1->CR1 |= I2C1_ACK; // Enable ACK
    	I2C1->DR = reg; // Register address
    }else {
    	I2C1->CR1 &= ~I2C1_ACK; // disable ACK
    	I2C1->DR = reg; // Register address
    }



    while (!(I2C1->SR1 & I2C1_BTF)); // wait until Byte transfer finished

    // 2. Repeated START

    I2C1->CR1 |= I2C1_START; // START

    while (!(I2C1->SR1 & I2C1_SB)){} // SB


    (void)I2C1->SR1;

    I2C1->DR = ((slave_address << 1) | 1); // Read

    while (!(I2C1->SR1 & I2C1_ADDR)); // ADDR

    I2C1->SR1;
    I2C1->SR2;

    I2C1->CR2 |= I2C1_DMAEN | I2C1_LAST;

    i2c_dma_receive(data,size);
}

void i2c_write(uint8_t slave_address, uint8_t *data, uint32_t size){
	 // Wait for bus is not busy
	while ((I2C1->SR2 & I2C1_SR2_BUSY));

   I2C1->CR1 |= I2C1_START; // START

   while (!(I2C1->SR1 & I2C1_SB)); // SB

   (void)I2C1->SR1;

   I2C1->DR = (slave_address << 1); // Write

   while (!(I2C1->SR1 & I2C1_ADDR)); // ADDR

   (void)I2C1->SR1;
   (void)I2C1->SR2;

   I2C1->CR2 |= I2C1_DMAEN | I2C1_LAST;

   i2c_dma_transfer(data, size);

}

void DMA1_Stream5_IRQHandler(){

	if ((DMA1->HISR & TCIF5)){ // TCIF5: receive complete

		// 5. STOP condition
		I2C1->CR1 |= I2C1_STOP; // STOP

		//do something
		DMA1->HIFCR |= TCIF5; // clear flag

	}

}


void DMA1_Stream6_IRQHandler(){


	if ((DMA1->HISR & TCIF6)){ // TCIF5: receive complete

			// 5. STOP condition
			I2C1->CR1 |= I2C1_STOP; // STOP

			//do something
			DMA1->HIFCR |= TCIF6; // clear

		}

}
