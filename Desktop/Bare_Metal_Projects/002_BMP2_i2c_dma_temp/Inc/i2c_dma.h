/*
 * i2c_dma.h
 *
 *  Created on: May 21, 2025
 *      Author: bilal
 */

#ifndef I2C_DMA_H_
#define I2C_DMA_H_

#include "stm32f4xx.h"
#include <stdint.h>
#include <stddef.h>
#define GPIOBEN              (1U<<1)

#define DMA1EN               (1U<<21)

void i2c1_init(void);
void i2c_tx_dma_stream6(void);
void i2c_rx_dma_stream5(void);
void i2c_dma_transfer(uint8_t *data, uint32_t size);
void i2c_dma_receive(uint8_t *buffer, uint32_t size);
void i2c_read(uint8_t slave_address, uint8_t reg, uint8_t *data, uint32_t size);
void i2c_write(uint8_t slave_address, uint8_t *data, uint32_t size);


#endif /* I2C_DMA_H_ */
