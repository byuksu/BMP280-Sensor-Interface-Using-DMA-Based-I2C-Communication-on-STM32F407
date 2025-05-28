#include "stm32f4xx.h"
#include "i2c_dma.h"
#include "bmp280.h"
#include <stdint.h>

int32_t temp_raw = 0;
int32_t press_raw = 0;

int main(void) {

	uint8_t id=0;
    i2c1_init();
    delay_ms(100);
    i2c_tx_dma_stream6();
    i2c_rx_dma_stream5();
    delay_ms(100);

    // BMP280 start
    i2c_read(BMP280_ADDR, BMP280_ID_REG, &id, 1);


    if (id != 0x58) {
        // ID is wrong!
        //do something
    }

    bmp280_init();

    delay_ms(100);

    while (1) {
        bmp280_read_raw_temp_press(&temp_raw, &press_raw);

        delay_ms(10);
    }
}
