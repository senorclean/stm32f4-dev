#include <stdint.h>
//#include "usart.h"
//#include "timer.h"
#include "gpio.h"
//#include "sched.h"
//#include "i2c.h"
//#include "usb.h"
#include "usb_conf.h"
#include "usbd_cdc_core.h"
#include "usbd_usr.h"
#include "usbd_desc.h"
#include "usbd_cdc_vcp.h"

/* TO DO: */ 
// configure USB1 for ACM capability (it can talk USART as well)
// use Wifi chip for SPI functionality
// put chip into low power modes and take it out

/***** Definitions ***********************************************************/
#define HSE_VALUE ((uint32_t)8000000)



__ALIGN_BEGIN USB_OTG_CORE_HANDLE  USB_OTG_dev __ALIGN_END;
/***** Main Loop *************************************************************/
int main(void) {
  gpio_init();
  uint8_t buf[255];
  uint8_t len;
  //tim3_init();
  //tim4_init();
  //tim5_init();
  //usart2_init();
  //dma_init();
  //i2c1_init();
  //usb_init();
  USBD_Init(&USB_OTG_dev,
  					USB_OTG_FS_CORE_ID,
  					&USR_desc,
  					&USBD_CDC_cb,
  					&USR_cb);

  

  while (1) {
    //scheduler();

  	len = VCP_get_string(&buf[0]);
  	if (len)
  		VCP_send_str(&buf[0]);

  }
}



