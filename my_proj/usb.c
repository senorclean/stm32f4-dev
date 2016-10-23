#include "usb.h"
#include "rcc.h"
#include "nvic.h"
#include "gpio.h"
#include "stdint.h"

typedef struct _USB_OTG_EP {
	uint8_t 			num;
	uint8_t 			is_in;
	uint8_t 			is_stall;
	uint8_t 			type;
	uint8_t 			data_pid_start;
	uint8_t 			even_odd_frame;
	uint16_t 			tx_fifo_num;
	uint32_t 			maxpacket;
	uint8_t 			*xfer_buff;
	uint32_t 			dma_addr;
	uint32_t 			xfer_len;
	uint32_t 			xfer_count;
	uint32_t 			rem_data_len;
	uint32_t 			total_data_len;
	uint32_t 			ctl_data_len;
} USB_OTG_EG, PUSB_OTG_EP;

typedef struct _USB_SETUP_REQ {
	uint8_t 	bmRequest;
	uint8_t 	bRequest;
	uint16_t 	wValue;
	uint16_t 	wIndex;
	uint16_t 	wLength;
} USB_SETUP_REQ;

typedef struct _Device_TypeDef {
	uint8_t  *(*GetDeviceDescriptor)(uint8_t speed, uint16_t *length);
	uint8_t  *(*GetLangIDStrDescriptor)(uint8_t speed, uint16_t *length);
	uint8_t  *(*GetManufacturerStrDescriptor)(uint8_t speed, uint16_t *length);
	uint8_t  *(*GetProductStrDescriptor)(uint8_t speed, uint16_t *length);
	uint8_t  *(*GetSerialStrDescriptor)(uint8_t speed, uint16_t *length);
	uint8_t  *(*GetConfigurationStrDescriptor)(uint8_t speed, uint16_t *length);
	uint8_t  *(*GetInterfaceStrDescriptor)(uint8_t speed, uint16_t *length);
} USBD_DEVICE, *pUSB_DEVICE;


typedef struct _DEVICE_CB {
	uint8_t 	(*Init)(void *pdev, uint8_t cfgidx);
	uint8_t 	(*DeInit)(void *pdev, uint8_t cfgidx);
	uint8_t 	(*Setup)(void *pdev, USB_SETUP_REQ *req);
	uint8_t 	(*EP0_TxSent)(void *pdev);
	uint8_t 	(*EP0_RxReady)(void *pdev);
	uint8_t 	(*DataIn)(void *pdev, uint8_t epnum);
	uint8_t 	(*DataOut)(void *pdev, uint8_t epnum);
	uint8_t		(*SOF)(void *pdev);
	uint8_t 	(*IsoINIncomplete)(void *pdev);
	uint8_t 	(*IsoOUTIncomplete)(void *pdev);
	uint8_t 	*(*GetConfigDescriptor)(uint8_t speed, uint16_t *length);
} USBD_Class_cb_TypeDef;

typedef struct _USB_USR_CB {
	void (*Init)();
	void (*DeviceReset)(uint8_t speed);
	void (*DeviceConfigured)();
	void (*DeviceSuspended)();
	void (*DeviceResumed)();
	void (*DeviceConnected)();
	void (*DeviceDisconnected)();
} USBD_Usr_cb_TypeDef;

typedef struct _DCD {
	uint8_t 								device_config;
	uint8_t 								device_state;
	uint8_t 								device_status;
	uint8_t 								device_address;
	uint32_t 								DevRemoteWakeup;
	USB_OTG_EP 							in_ep [15];
	USB_OTG_EP 							out_ep [15];
	uint8_t 								setup_packet[8*3];
	USBD_Class_cb_TypeDef		*class_cb;
	USBD_Usr_cb_TypeDef 		*usr_cb;
	USBD_DEVICE 						*usr_device;
	uint8_t 								*pConfig_descriptor;
} DCD_DEV , *DCD_PDEV;

typedef struct _USB_OTG_handle {
	DCD_DEV 	dev;
} USB_OTG_CORE_HANDLE, *PUSB_OTG_CORE_HANDLE;


USB_OTG_CORE_HANDLE pdev;
USBD_DEVICE pDevice;
USBD_Class_cb_TypeDef class_cb;
USBD_Usr_cb_TypeDef usr_cb;

void OTG_FS_IRQHandler() {

}


void usb_init() {

	// configure OTG FS clock for 48 MHz (this is done in system_stm32f4xx.c)
	// VCO freq = PLL input freq (8 MHz) * (PLL_N (336) / PLL_M (8))
	// USB_OTG_FS freq (needs to be 48 MHz) = VCO freq (336 MHz) / PLLQ

	// configure pins for USB
	// AF10 for PA8 (SOF), PA9 (VBUS), PA11 (D-), and PA12 (D+)
	GPIO_MODER(GPIOA) |= GPIO_MODE(8, GPIO_MODE_AF);
	GPIO_MODER(GPIOA) |= GPIO_MODE(9, GPIO_MODE_AF);
  GPIO_MODER(GPIOA) |= GPIO_MODE(11, GPIO_MODE_AF);
  GPIO_MODER(GPIOA) |= GPIO_MODE(12, GPIO_MODE_AF);

  GPIO_AFRH(GPIOA) |= GPIO_AFR(0, GPIO_AF10);              
  GPIO_AFRH(GPIOA) |= GPIO_AFR(1, GPIO_AF10);              
  GPIO_AFRH(GPIOA) |= GPIO_AFR(3, GPIO_AF10);
  GPIO_AFRH(GPIOA) |= GPIO_AFR(4, GPIO_AF10);


  // enable system clock cfg controller
	RCC_APB2ENR |= RCC_APB2ENR_SYSCFGEN;
	// enable USB OTG FS clock
	RCC_AHB2ENR |= RCC_AHB2ENR_OTGFSEN;
	// enable pwr clock
	RCC_APB1RSTR |= RCC_APB1RSTR_PWRRST;



	OTG_FS_GUSBCFG = 0;
	OTG_FS_GCCFG = 0;
	OTG_FS_GAHBCFG = 0;


	pdev->dev.class_cb = class_cb;
	pdev->dev.usr_cb = usr_cb;
	pdev->dev.usr_device = pDevice;


	ms = 3;
  while (ms-- > 0) {		
     volatile int x=5971;		
     while (x-- > 0)		
        __asm("nop");		
  }


  NVIC_IPR((NVIC_OTG_FS_IRQ / 4) + (NVIC_OTG_FS_IRQ % 4)) = 0x20;
  NVIC_ISER(NVIC_OTG_FS_IRQ / 32) = (1 << (NVIC_OTG_FS_IRQ % 32));

  
}