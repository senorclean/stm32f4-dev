#ifndef USB_H_
#define USB_H_

#include "memorymap.h"

/***** USB_OTG_FS Global Regs ***********************************************/
#define OTG_FS_GOTGCTL					 				MMIO32(USB_OTG_FS_BASE + 0x000)
#define OTG_FS_GOTGINT					 				MMIO32(USB_OTG_FS_BASE + 0x004)
#define OTG_FS_GAHBCFG           				MMIO32(USB_OTG_FS_BASE + 0x008)
#define OTG_FS_GUSBCFG 									MMIO32(USB_OTG_FS_BASE + 0x00C)
#define OTG_FS_GRSTCTL 									MMIO32(USB_OTG_FS_BASE + 0x010)
#define OTG_FS_GINTSTS 									MMIO32(USB_OTG_FS_BASE + 0x014)
#define OTG_FS_GINTMSK         					MMIO32(USB_OTG_FS_BASE + 0x018)
#define OTG_FS_GRXSTSR           				MMIO32(USB_OTG_FS_BASE + 0x01C)
#define OTG_FS_GRXSTSP           				MMIO32(USB_OTG_FS_BASE + 0x020)
#define OTG_FS_GRXFSIZ           				MMIO32(USB_OTG_FS_BASE + 0x024)

#define OTG_FS_GCCFG           					MMIO32(USB_OTG_FS_BASE + 0x038)
#define OTG_FS_CID          		 				MMIO32(USB_OTG_FS_BASE + 0x03C)
#define OTG_FS_DIEPTXF           				MMIO32(USB_OTG_FS_BASE + 0x104)

/***** USB_OTG_FS Device Regs **********************************************/
#define OTG_FS_DCFG           					MMIO32(USB_OTG_FS_BASE + 0x800)
#define OTG_FS_DCTL           					MMIO32(USB_OTG_FS_BASE + 0x804)
#define OTG_FS_DSTS           					MMIO32(USB_OTG_FS_BASE + 0x808)
#define OTG_FS_DIEPMSK           				MMIO32(USB_OTG_FS_BASE + 0x810)
#define OTG_FS_DOEPMSK           				MMIO32(USB_OTG_FS_BASE + 0x814)
#define OTG_FS_DAINT           					MMIO32(USB_OTG_FS_BASE + 0x818)
#define OTG_FS_DAINTMSK           			MMIO32(USB_OTG_FS_BASE + 0x81C)
#define OTG_FS_DVBUSDIS           			MMIO32(USB_OTG_FS_BASE + 0x828)
#define OTG_FS_DVBUSPULSE           		MMIO32(USB_OTG_FS_BASE + 0x82C)
#define OTG_FS_DIEPEMPMSK           		MMIO32(USB_OTG_FS_BASE + 0x834)
#define OTG_FS_DIEPCTL0           			MMIO32(USB_OTG_FS_BASE + 0x900)
#define OTG_FS_DIEPCTL           				MMIO32(USB_OTG_FS_BASE + 0x900)
#define OTG_FS_DOEPCTL0           			MMIO32(USB_OTG_FS_BASE + 0xB00)
#define OTG_FS_DOEPCTL           				MMIO32(USB_OTG_FS_BASE + 0xB00)
#define OTG_FS_DIEPINT           				MMIO32(USB_OTG_FS_BASE + 0x908)
#define OTG_FS_DOEPINT           				MMIO32(USB_OTG_FS_BASE + 0xB08)
#define OTG_FS_DIEPTSIZ0           			MMIO32(USB_OTG_FS_BASE + 0x910)
#define OTG_FS_DOEPTSIZ0           			MMIO32(USB_OTG_FS_BASE + 0xB10)
#define OTG_FS_DIEPTSIZ           			MMIO32(USB_OTG_FS_BASE + 0x910)
#define OTG_FS_DTXFSTS           				MMIO32(USB_OTG_FS_BASE + 0x918)
#define OTG_FS_DOEPTSIZ           			MMIO32(USB_OTG_FS_BASE + 0xB10)

// Need to update regs like DIEPCTL for 1-3
// Don't feel like typing out all of the reg bits right now. Will do later


/***** Variables and Function Prototypes **********************************/
void usb_init();

#endif