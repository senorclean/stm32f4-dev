#include "usb.h"
#include "rcc.h"
#include "nvic.h"
#include "gpio.h"

int usbFlag = 0;

void OTG_FS_IRQHandler() {
	//GPIO_ODR(GPIOD) |= (1 << 14);
	if (OTG_FS_GINTSTS & (1 << 12)) {
		GPIO_ODR(GPIOD) |= (1 << 13);
		//usbFlag = 1;
	}
 // check for reset flag and blink LED
}


void usb_init() {

	// configure OTG FS clock for 48 MHz (this is done in system_stm32f4xx.c)
	// VCO freq = PLL input freq (8 MHz) * (PLL_N (336) / PLL_M (8))
	// USB_OTG_FS freq (needs to be 48 MHz) = VCO freq (336 MHz) / PLLQ

	// configure pins for USB
	// AF10 for PA9 (VBUS), PA10 (ID), PA11 (D-), and PA12 (D+)

	GPIO_MODER(GPIOA) |= GPIO_MODE(9, GPIO_MODE_AF);
  GPIO_MODER(GPIOA) |= GPIO_MODE(10, GPIO_MODE_AF);
  GPIO_MODER(GPIOA) |= GPIO_MODE(11, GPIO_MODE_AF);
  GPIO_MODER(GPIOA) |= GPIO_MODE(12, GPIO_MODE_AF);

  GPIO_AFRH(GPIOA) |= GPIO_AFR(1, GPIO_AF10); 
  GPIO_AFRH(GPIOA) |= GPIO_AFR(2, GPIO_AF10);              
  GPIO_AFRH(GPIOA) |= GPIO_AFR(3, GPIO_AF10);
  GPIO_AFRH(GPIOA) |= GPIO_AFR(4, GPIO_AF10);


  // enable system clock cfg controller
	RCC_APB2ENR |= RCC_APB2ENR_SYSCFGEN;
	// enable USB OTG FS clock
	RCC_AHB2ENR |= RCC_AHB2ENR_OTGFSEN;
	// reset the power clock? this is in the stm usb init code
	RCC_APB1RSTR |= RCC_APB1RSTR_PWRRST;

  // core initialization

  // in OTG_FS_GAHBCFG reg
  	// set GINTMSK bit
  	// RXFLVL bit in OTG_FS_GINTSTS (RxFIFO is non-empty)
  	// periodic TxFIFO empty level
  // in OTG_FS_GUSBCFG
  	// HNP capable bit
  	// SRP capable bit
  	// FS timeout calibration field
  	// USB turnaround time field
  // in OTG_FS_GINTMSK
  	// unmask OTG interrupt
  	// unmask mode mismatch interrupt mask


	OTG_FS_GUSBCFG |= (1 << 6);
	OTG_FS_GRSTCTL |= (1 << 0);

	int ms = 100;
  while (ms-- > 0) {		
     volatile int x=5971;		
     while (x-- > 0)		
        __asm("nop");		
  }

  OTG_FS_GAHBCFG |= (1 << 0);
  OTG_FS_GAHBCFG |= (1 << 7);
  OTG_FS_GINTSTS |= (1 << 7);

  OTG_FS_GUSBCFG &= ~(1 << 9);
  OTG_FS_GUSBCFG |= (1 << 8);
  OTG_FS_GUSBCFG |= (6 << 10);
  // not really sure what to do for this timeout calibration value
  OTG_FS_GUSBCFG &= ~(7 << 0);

  OTG_FS_GINTMSK |= (1 << 2);
  OTG_FS_GINTMSK |= (1 << 1);

  // device initialization
  // OTG_FS_DCFG reg
  	// device speed
  	// non-zero length status OUT handshake
  // OTG_FS_GINTMSK to unmask:
  	// USB reset
  	// enumeration done
  	// early suspend
  	// USB suspend
  	// SOF

  OTG_FS_DCFG |= (3 << 0);
  OTG_FS_DCFG &= ~(1 << 2);

  OTG_FS_GINTMSK |= (1 << 11);
  OTG_FS_GINTMSK |= (1 << 10);
  OTG_FS_GINTMSK |= (1 << 13);
  OTG_FS_GINTMSK |= (1 << 12);

  // for some reason the start of frame bit is constantly active and
  // firing the USB interrupt over all other interrupts. when deactivated,
  // I can get the USB interrupt firing correctly in the ISR. Hopefully
  // once I set up the rest of the configuration set up for the device
  // registers this won't be an issue
  //OTG_FS_GINTMSK |= (1 << 3);

  // set VBUSBSEN in OTG_FS_GCCFG

  OTG_FS_GCCFG |= (1 << 19);


  NVIC_IPR((NVIC_OTG_FS_IRQ / 4) + (NVIC_OTG_FS_IRQ % 4)) = 0x20;
  NVIC_ISER(NVIC_OTG_FS_IRQ / 32) = (1 << (NVIC_OTG_FS_IRQ % 32));

  // wait for USBRST interrupt
  //while(usbFlag != 1);


  // wait for ENUMDNE interrupt (end of reset on USB) and read OTG_FS_DSTS
  // to determine enumeration speed and perform steps listed on pg. 1353
	
}