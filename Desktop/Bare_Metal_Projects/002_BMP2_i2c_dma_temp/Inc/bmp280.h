/*
 * bmp280.h
 *
 *  Created on: May 23, 2025
 *      Author: bilal
 */

#ifndef BMP280_H_
#define BMP280_H_

#include <stdint.h>

#define BMP280_ADDR           0x76 //slave address
#define BMP280_ID_REG         0xD0 // the chip id number chip_id[7:0], which is 0x58.
#define BMP280_RESET_REG      0xE0 //reset address: by writing 0xB6
#define BMP280_CTRL_MEAS_REG  0xF4 //Bit 7, 6, 5 : Temperature oversampling
#define BMP280_CONFIG_REG     0xF5
#define BMP280_PRESS_MSB_REG  0xF7
#define BMP280_TEMP_MSB_REG   0xFA //FA to FC 20 bit "0xFC (bit 7, 6, 5, 4)"


void bmp280_init(void);
uint8_t bmp280_read_id(void);
void bmp280_reset(void);
void bmp280_read_raw_temp_press(int32_t *temp_raw, int32_t *press_raw);
void bmp280_read_calibration_data(void);
int32_t bmp280_compensate_temperature(int32_t adc_T);
uint32_t bmp280_compensate_pressure(int32_t adc_P);
void delay_ms(volatile uint32_t ms);

#endif /* BMP280_H_ */
