#include "bmp280.h"
#include "i2c_dma.h"

uint8_t 					bmp280_id;

unsigned short 				dig_T1Data=0;
short					 	dig_T2Data=0;
short					 	dig_T3Data=0;
double 						var1;
double 						var2;
double 						temperature;


void bmp280_read_calibration(void) {

	uint8_t temp[2];

	i2c_read(BMP280_ADDR, 0x88, temp, 2);
	dig_T1Data = (uint16_t)(temp[1] << 8) | temp[0];

	i2c_read(BMP280_ADDR, 0x8A, temp, 2);
	dig_T2Data = (uint16_t)(temp[1] << 8) | temp[0];

	i2c_read(BMP280_ADDR, 0x8C, temp, 2);
	dig_T3Data = (uint16_t)(temp[1] << 8) | temp[0];

}


uint32_t bmp280_compensate_temp(signed long data)
{
	var1=((data*1.0)/(16384.0)-(dig_T1Data*1.0)/1024.0)*(dig_T2Data*1.0);
	var2=(((data*1.0)/(131072.0)-(dig_T1Data*1.0)/8192.0)*((data*1.0)/(131072.0)-(dig_T1Data*1.0)/8192.0))*(dig_T3Data*1.0);
	temperature=(var1+var2)/2;
	return temperature/5120.0;

}

void delay_ms(volatile uint32_t ms) {
    for (volatile uint32_t i = 0; i < ms * 16000; i++) {
        __NOP();
    }
}

uint8_t bmp280_read_id(void) {
    uint8_t id = 0;
    i2c_read(BMP280_ADDR, BMP280_ID_REG, &id, 1);
    return id;
}

void bmp280_reset(void) {

    uint8_t reset_cmd = 0xB6;
    i2c_write(BMP280_ADDR, &((uint8_t[]){BMP280_RESET_REG, reset_cmd})[0], 2);

}

void bmp280_init(void) {


    // Reset
    bmp280_reset();

    delay_ms(10);
/*
    // standby 500ms : Bit 7, 6, 5 = 100 , filter x4 : Bit 4, 3, 2 = 0x10
    uint8_t config[2] = {BMP280_CONFIG_REG, 0b10001000};
    i2c_write(BMP280_ADDR, config, 2);
*/
    // ctrl_meas (Bit 7, 6, 5 for temp): normal mode:11, temp x1 : 001, press x1: 001
    uint8_t ctrl_meas[2] = {BMP280_CTRL_MEAS_REG, 0x27};
    i2c_write(BMP280_ADDR, ctrl_meas, 2);
    bmp280_read_calibration();
}

void bmp280_read_raw_temp_press(int32_t *temp, int32_t *press) {
    uint8_t buf[3];
    i2c_read(BMP280_ADDR, 0xFA, buf, 3);

    //int32_t adc_P = ((int32_t)buf[0] << 12) | ((int32_t)buf[1] << 4) | (buf[2] >> 4);
    int32_t adc_T = ((int32_t)buf[0] << 12) | ((int32_t)buf[1] << 4) | (buf[2] >> 4);

    *temp = bmp280_compensate_temp(adc_T);
    //*press = bmp280_compensate_pressure(adc_P);
}
