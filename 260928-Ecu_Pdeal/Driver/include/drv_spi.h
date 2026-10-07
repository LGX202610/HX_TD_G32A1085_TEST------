#ifndef DRV_SPI_H
#define DRV_SPI_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes */
#include <stdint.h>


/* Define */




/* Enum */ 



/* Struct */ 





void drv_spi1_init(void);
void drv_spi1_sleep(void);
void drv_spi1_wakeup(void);
void drv_spi1_enable_nss(void);
void drv_spi1_disable_nss(void);
uint8_t drv_spi1_rw_data(uint8_t *tx_buf, uint8_t *rx_buf, uint16_t len);


#ifdef __cplusplus
}
#endif

#endif /*  */
