#ifndef TIMER_H_
#define TIMER_H_

#include "stdint.h"
#include "memorymap.h"

/***** Timer Base Regs ********************************************/
#define TIM1 							TIM1_BASE	
#define TIM2 							TIM2_BASE	
#define TIM3 							TIM3_BASE	
#define TIM4 							TIM4_BASE	
#define TIM5 							TIM5_BASE	
#define TIM6 							TIM6_BASE	
#define TIM7 							TIM7_BASE	
#define TIM8 							TIM8_BASE	
#define TIM9 							TIM9_BASE	
#define TIM10 						TIM10_BASE	
#define TIM11 						TIM11_BASE	
#define TIM12 						TIM12_BASE	
#define TIM13 						TIM13_BASE	
#define TIM14 						TIM14_BASE	

/***** Timer Regs ************************************************/
#define TIM_CR1(tim_base) 			MMIO16(tim_base + 0x00)
// #define TIM1_CR1 				TIM1_CR1(TIM1)

#define TIM_CR2(tim_base) 			MMIO16(tim_base + 0x04)
#define TIM_SMCR(tim_base) 			MMIO16(tim_base + 0x08)
#define TIM_DIER(tim_base) 			MMIO16(tim_base + 0x0C)
#define TIM_SR(tim_base) 				MMIO16(tim_base + 0x10)
#define TIM_EGR(tim_base) 			MMIO16(tim_base + 0x14)
#define TIM_CCMR1(tim_base)			MMIO16(tim_base + 0x18)
#define TIM_CCMR2(tim_base)			MMIO16(tim_base + 0x1C)
#define TIM_CCER(tim_base) 			MMIO16(tim_base + 0x20)
#define TIM_CNT(tim_base) 			MMIO32(tim_base + 0x24)
#define TIM_PSC(tim_base) 			MMIO16(tim_base + 0x28)
#define TIM_ARR(tim_base) 			MMIO32(tim_base + 0x2C)
#define TIM_RCR(tim_base) 			MMIO16(tim_base + 0x30)
#define TIM_CCR1(tim_base) 			MMIO32(tim_base + 0x34)
#define TIM_CCR2(tim_base) 			MMIO32(tim_base + 0x38)
#define TIM_CCR3(tim_base) 			MMIO32(tim_base + 0x3C)
#define TIM_CCR4(tim_base) 			MMIO32(tim_base + 0x40)
#define TIM_BDTR(tim_base) 			MMIO16(tim_base + 0x44)
#define TIM_DCR(tim_base) 			MMIO16(tim_base + 0x48)
#define TIM_DMAR(tim_base) 			MMIO16(tim_base + 0x4C)

/***** Timer CR1 Regs ************************************************/
#define TIM_CR1_CKD 								(0 << 8)
#define TIM_CR1_CKD_MUL_2 					(1 << 8)
#define TIM_CR1_CKD_MUL_4 					(2 << 8)

#define TIM_CR1_APRE 								(1 << 7)

#define TIM_CR1_CMS_EDGE 						(0 << 5)
#define TIM_CR1_CMS_CENTER_1				(1 << 5)
#define TIM_CR1_CMS_CENTER_2 				(2 << 5)
#define TIM_CR1_CMS_CENTER_3 				(3 << 5)

#define TIM_CR1_DIR 		 						(1 << 4)

#define TIM_CR1_OPM 								(1 << 3)
#define TIM_CR1_URS 								(1 << 2)
#define TIM_CR1_UDIS 								(1 << 1)
#define TIM_CR1_CEN 								(1 << 0)

/***** Timer CR2 Regs ************************************************/
#define TIM_CR2_TI1S 				(1 << 7)

#define TIM_CR2_MMS_RST 			(0 << 4)
#define TIM_CR2_MMS_EN 				(1 << 4)
#define TIM_CR2_MMS_UPDATE 			(2 << 4)
#define TIM_CR2_CMP_PULSE			(3 << 4)
#define TIM_CR2_CMP_OC1REF 			(4 << 4)
#define TIM_CR2_CMP_OC2REF 			(5 << 4)
#define TIM_CR2_CMP_OC3REF 			(6 << 4)
#define TIM_CR2_CMP_OC4REF 			(7 << 4)

#define TIM_CR2_CCDS 				(1 << 3)

/***** Timer SMCR Regs ***********************************************/
#define TIM_SMCR_ETP 				(1 << 15)
#define TIM_SMCR_ECE 				(1 << 14)

#define TIM_SMCR_ETPS_DIV_2 		(1 << 12)
#define TIM_SMCR_ETPS_DIV_4 		(2 << 12)
#define TIM_SMCR_ETPS_DIV_8 		(3 << 12)

#define TIM_SMCR_ETF_OFF 			(0 << 8)
#define TIM_SMCR_ETF_CK_INT_N_2 	(1 << 8)
#define TIM_SMCR_ETF_CK_INT_N_4 	(2 << 8)
#define TIM_SMCR_ETF_CK_INT_N_8 	(3 << 8)
#define TIM_SMCR_ETF_DTS_DIV_2_N_6 	(4 << 8)
#define TIM_SMCR_ETF_DTS_DIV_2_N_8 	(5 << 8)
#define TIM_SMCR_ETF_DTS_DIV_4_N_6 	(6 << 8)
#define TIM_SMCR_ETF_DTS_DIV_4_N_8 	(7 << 8)
#define TIM_SMCR_ETF_DTS_DIV_8_N_6 	(8 << 8)
#define TIM_SMCR_ETF_DTS_DIV_8_N_8 	(9 << 8)
#define TIM_SMCR_ETF_DTS_DIV_16_N_5 (10 << 8)
#define TIM_SMCR_ETF_DTS_DIV_16_N_6	(11 << 8)
#define TIM_SMCR_ETF_DTS_DIV_16_N_8	(12 << 8)
#define TIM_SMCR_ETF_DTS_DIV_32_N_5	(13 << 8)
#define TIM_SMCR_ETF_DTS_DIV_32_N_6	(14 << 8)
#define TIM_SMCR_ETF_DTS_DIV_32_N_8	(15 << 8)

#define TIM_SMCR_MSM 				(1 << 7)

#define TIM_SMCR_TS_ITR0 			(0 << 4)
#define TIM_SMCR_TS_ITR1			(1 << 4)
#define TIM_SMCR_TS_ITR2 			(2 << 4)
#define TIM_SMCR_TS_ITR3 			(3 << 4)
#define TIM_SMCR_TS_TI1F_ED 		(4 << 4)
#define TIM_SMCR_TS_TI1FP1 			(5 << 4)
#define TIM_SMCR_TS_T12FP2 			(6 << 4)
#define TIM_SMCR_TS_ETRF 			(7 << 4)

#define TIM_SMCR_MSM_OFF 			(0 << 0)
#define TIM_SMCR_SMS_ENC1 			(1 << 0)
#define TIM_SMCR_SMS_ENC2 			(2 << 0)
#define TIM_SMCR_SMS_ENC3 			(3 << 0)
#define TIM_SMCR_SMS_RST_MODE 		(4 << 0)
#define TIM_SMCR_SMS_GATED_MODE 	(5 << 0)
#define TIM_SMCR_SMS_TRIG_MODE 		(6 << 0)
#define TIM_SMCR_SMS_EXT_CLK_MODE 	(7 << 0)

/***** Timer DIER Regs ***************************************************/
#define TIM_DIER_TDE 				(1 << 14)
#define TIM_DIER_CC4DE 				(1 << 12)
#define TIM_DIER_CC3DE 				(1 << 11)
#define TIM_DIER_CC2DE 				(1 << 10)
#define TIM_DIER_CC1DE 				(1 << 9)
#define TIM_DIER_UDE 				(1 << 8)
#define TIM_DIER_TIE 				(1 << 6)
#define TIM_DIER_CC4IE 				(1 << 4)
#define TIM_DIER_CC3IE 				(1 << 3)
#define TIM_DIER_CC2IE 				(1 << 2)
#define TIM_DIER_CC1IE 				(1 << 1)
#define TIM_DIER_UIE 				(1 << 0)

/***** Timer SR Regs *****************************************************/
#define TIM_SR_CC4OF 				(1 << 12)
#define TIM_SR_CC3OF 				(1 << 11)
#define TIM_SR_CC2OF 				(1 << 10)
#define TIM_SR_CC1OF 				(1 << 9)
#define TIM_SR_TIF 	 				(1 << 6)
#define TIM_SR_CC4IF 				(1 << 4)
#define TIM_SR_CC3IF 				(1 << 3)
#define TIM_SR_CC2IF 				(1 << 2)
#define TIM_SR_CC1IF 				(1 << 1)
#define TIM_SR_UIF  				(1 << 0)

/***** Timer EGR Regs ****************************************************/
#define TIM_EGR_TG 					(1 << 6)
#define TIM_EGR_CC4G 				(1 << 4)
#define TIM_EGR_CC3G 				(1 << 3)
#define TIM_EGR_CC2G 				(1 << 2)
#define TIM_EGR_CC1G 				(1 << 1)
#define TIM_EGR_UG 					(1 << 0)

/***** Timer CCMR1 Regs **************************************************/

/* Output compare Mode */
#define TIM_CCMR1_OC2CE				(1 << 15)

#define TIM_CCMR1_OC2M_FROZEN 		(0 << 12)
#define TIM_CCMR1_OC2M_ACTIVE		(1 << 12)
#define TIM_CCMR1_OC2M_INACTIVE		(2 << 12)
#define TIM_CCMR1_OC2M_TOGGLE		(3 << 12)
#define TIM_CCMR1_OC2M_FORCE_LOW	(4 << 12)
#define TIM_CCMR1_OC2M_FORCE_HIGH	(5 << 12)
#define TIM_CCMR1_OC2M_PWM1			(6 << 12)
#define TIM_CCMR1_OC2M_PWM2			(7 << 12)

#define TIM_CCMR1_OC2PE				(1 << 11)
#define TIM_CCMR1_OC2FE				(1 << 10)

#define TIM_CCMR1_CC2S_OUT 			(0 << 8)
#define TIM_CCMR1_CC2S_IN_TI2		(1 << 8)
#define TIM_CCMR1_CC2S_IN_TI1		(2 << 8)
#define TIM_CCMR1_CC2S_IN_TRC		(3 << 8)

#define TIM_CCMR1_OC1CE             (1 << 7)

#define TIM_CCRM1_OC1M_FROZEN 		(0 << 4)
#define TIM_CCMR1_OC1M_ACTIVE		(1 << 4)
#define TIM_CCMR1_OC1M_INACTIVE		(2 << 4)
#define TIM_CCMR1_OC1M_TOGGLE		(3 << 4)
#define TIM_CCMR1_OC1M_FORCE_LOW	(4 << 4)
#define TIM_CCMR1_OC1M_FORCE_HIGH	(5 << 4)
#define TIM_CCMR1_OC1M_PWM1			(6 << 4)
#define TIM_CCMR1_OC1M_PWM2			(7 << 4)

#define TIM_CCMR1_OC1PE				(1 << 3)
#define TIM_CCMR1_OC1FE				(1 << 2)

#define TIM_CCMR1_CC1S_OUT 			(0 << 0)
#define TIM_CCMR1_CC1S_IN_TI1		(1 << 0)
#define TIM_CCMR1_CC1S_IN_TI2		(2 << 0)
#define TIM_CCMR1_CC1S_IN_TRC		(3 << 0)

/* Input capture mode */
#define TIM_CCMR1_IC2F_OFF 				(0 << 12)
#define TIM_CCMR1_IC2F_CK_INT_N_2		(1 << 12)
#define TIM_CCMR1_IC2F_CK_INT_N_4		(2 << 12)
#define TIM_CCMR1_IC2F_CK_INT_N_8		(3 << 12)
#define TIM_CCMR1_IC2F_DTF_DIV_2_N_6 	(4 << 12)
#define TIM_CCMR1_IC2F_DTF_DIV_2_N_8	(5 << 12)
#define TIM_CCMR1_IC2F_DTF_DIV_4_N_6	(6 << 12)
#define TIM_CCMR1_IC2F_DTF_DIV_4_N_8	(7 << 12)
#define TIM_CCMR1_IC2F_DTF_DIV_8_N_6	(8 << 12)
#define TIM_CCMR1_IC2F_DTF_DIV_8_N_8	(9 << 12)
#define TIM_CCMR1_IC2F_DTF_DIV_16_N_5	(10 << 12)
#define TIM_CCMR1_IC2F_DTF_DIV_16_N_6	(11 << 12)
#define TIM_CCMR1_IC2F_DTF_DIV_16_N_8	(12 << 12)
#define TIM_CCMR1_IC2F_DTF_DIV_32_N_5	(13 << 12)
#define TIM_CCMR1_IC2F_DTF_DIV_32_N_6	(14 << 12)
#define TIM_CCMR1_IC2F_DTF_DIV_32_N_8	(15 << 12)

#define TIM_CCMR1_IC2PSC_OFF 		(0 << 10)
#define TIM_CCMR1_IC2PSC_2			(1 << 10)
#define TIM_CCMR1_IC2PSC_4			(2 << 10)
#define TIM_CCMR1_IC2PSC_8			(3 << 10)

#define TIM_CCMR1_IC1F_OFF 				(0 << 4)
#define TIM_CCMR1_IC1F_CK_INT_N_2		(1 << 4)
#define TIM_CCMR1_IC1F_CK_INT_N_4		(2 << 4)
#define TIM_CCMR1_IC1F_CK_INT_N_8		(3 << 4)
#define TIM_CCMR1_IC1F_DTF_DIV_2_N_6	(4 << 4)
#define TIM_CCMR1_IC1F_DTF_DIV_2_N_8	(5 << 4)
#define TIM_CCMR1_IC1F_DTF_DIV_4_N_6	(6 << 4)
#define TIM_CCMR1_IC1F_DTF_DIV_4_N_8	(7 << 4)
#define TIM_CCMR1_IC1F_DTF_DIV_8_N_6	(8 << 4)
#define TIM_CCMR1_IC1F_DTF_DIV_8_N_8	(9 << 4)
#define TIM_CCMR1_IC1F_DTF_DIV_16_N_5	(10 << 4)
#define TIM_CCMR1_IC1F_DTF_DIV_16_N_6	(11 << 4)
#define TIM_CCMR1_IC1F_DTF_DIV_16_N_8	(12 << 4)
#define TIM_CCMR1_IC1F_DTF_DIV_32_N_5	(13 << 4)
#define TIM_CCMR1_IC1F_DTF_DIV_32_N_6	(14 << 4)
#define TIM_CCMR1_IC1F_DTF_DIV_32_N_8	(15 << 4)

#define TIM_CCMR1_IC1PSC_OFF 		(0 << 2)
#define TIM_CCMR1_IC1PSC_2			(1 << 2)
#define TIM_CCMR1_IC1PSC_4			(2 << 2)
#define TIM_CCMR1_IC1PSC_8			(3 << 2)

/***** Timer CCMR2 Regs **************************************************/

/* Output compare mode */
#define TIM_CCMR2_OC4CE				(1 << 15)

#define TIM_CCMR2_OC4M_FROZEN		(0 << 12)
#define TIM_CCMR2_OC4M_ACTIVE		(1 << 12)
#define TIM_CCMR2_OC4M_INACTIVE		(2 << 12)
#define TIM_CCMR2_OC4M_TOGGLE		(3 << 12)
#define TIM_CCMR2_OC4M_FORCE_LOW	(4 << 12)
#define TIM_CCMR2_OC4M_FORCE_HIGH	(5 << 12)
#define TIM_CCMR2_OC4M_PWM1			(6 << 12)
#define TIM_CCMR2_OC4M_PWM2			(7 << 12)

#define TIM_CCMR2_OC4PE				(1 << 11)
#define TIM_CCMR2_OC4FE				(1 << 10)

#define TIM_CCMR2_CC4S_OUT 			(0 << 8)
#define TIM_CCMR2_CC4S_IN_TI2		(1 << 8)
#define TIM_CCMR2_CC4S_IN_TI1		(2 << 8)
#define TIM_CCMR2_CC4S_IN_TRC		(3 << 8)

#define TIM_CCMR2_OC3CE				(1 << 7)

#define TIM_CCMR2_OC3M_FROZEN 		(0 << 4)
#define TIM_CCMR2_OC3M_ACTIVE		(1 << 4)
#define TIM_CCMR2_OC3M_INACTIVE		(2 << 4)
#define TIM_CCMR2_OC3M_TOGGLE		(3 << 4)
#define TIM_CCMR2_OC3M_FORCE_LOW	(4 << 4)
#define TIM_CCMR2_OC3M_FORCE_HIGH	(5 << 4)
#define TIM_CCMR2_OC3M_PWM1			(6 << 4)
#define TIM_CCMR2_OC3M_PWM2			(7 << 4)

#define TIM_CCMR2_OC3PE				(1 << 3)
#define TIM_CCMR2_OC3FE				(1 << 2)

#define TIM_CCMR2_CC3S_OUT 			(0 << 0)
#define TIM_CCMR2_CC3S_IN_TI2		(1 << 0)
#define TIM_CCMR2_CC3S_IN_TI1		(2 << 0)
#define TIM_CCMR2_CC3S_IN_TRC		(3 << 0)

/* Input capture mode */
#define TIM_CCMR2_IC4F_OFF 				(0 << 12)
#define TIM_CCMR2_IC4F_CK_INT_N_2		(1 << 12)
#define TIM_CCMR2_IC4F_CK_INT_N_4		(2 << 12)
#define TIM_CCMR2_IC4F_CK_INT_N_8		(3 << 12)
#define TIM_CCMR2_IC4F_DTF_DIV_2_N_6	(4 << 12)
#define TIM_CCMR2_IC4F_DTF_DIV_2_N_8	(5 << 12)
#define TIM_CCMR2_IC4F_DTF_DIV_4_N_6	(6 << 12)
#define TIM_CCMR2_IC4F_DTF_DIV_4_N_8	(7 << 12)
#define TIM_CCMR2_IC4F_DTF_DIV_8_N_6	(8 << 12)
#define TIM_CCMR2_IC4F_DTF_DIV_8_N_8	(9 << 12)
#define TIM_CCMR2_IC4F_DTF_DIV_16_N_5	(10 << 12)
#define TIM_CCMR2_IC4F_DTF_DIV_16_N_6	(11 << 12)
#define TIM_CCMR2_IC4F_DTF_DIV_16_N_8	(12 << 12)
#define TIM_CCMR2_IC4F_DTF_DIV_32_N_5	(13 << 12)
#define TIM_CCMR2_IC4F_DTF_DIV_32_N_6	(14 << 12)
#define TIM_CCMR2_IC4F_DTF_DIV_32_N_8	(15 << 12)

#define TIM_CCMR2_IC4PSC_OFF 		(0 << 10)
#define TIM_CCMR2_IC4PSC_2			(1 << 10)
#define TIM_CCMR2_IC4PSC_4			(2 << 10)
#define TIM_CCMR2_IC4PSC_8			(3 << 10)

#define TIM_CCMR2_IC3F_OFF 				(0 << 4)
#define TIM_CCMR2_IC3F_CK_INT_N_2		(1 << 4)
#define TIM_CCMR2_IC3F_CK_INT_N_4		(2 << 4)
#define TIM_CCMR2_IC3F_CK_INT_N_8		(3 << 4)
#define TIM_CCMR2_IC3F_DTF_DIV_2_N_6	(4 << 4)
#define TIM_CCMR2_IC3F_DTF_DIV_2_N_8	(5 << 4)
#define TIM_CCMR2_IC3F_DTF_DIV_4_N_6	(6 << 4)
#define TIM_CCMR2_IC3F_DTF_DIV_4_N_8	(7 << 4)
#define TIM_CCMR2_IC3F_DTF_DIV_8_N_6	(8 << 4)
#define TIM_CCMR2_IC3F_DTF_DIV_8_N_8	(9 << 4)
#define TIM_CCMR2_IC3F_DTF_DIV_16_N_5	(10 << 4)
#define TIM_CCMR2_IC3F_DTF_DIV_16_N_6	(11 << 4)
#define TIM_CCMR2_IC3F_DTF_DIV_16_N_8	(12 << 4)
#define TIM_CCMR2_IC3F_DTF_DIV_32_N_5	(13 << 4)
#define TIM_CCMR2_IC3F_DTF_DIV_32_N_6	(14 << 4)
#define TIM_CCMR2_IC3F_DTF_DIV_32_N_8	(15 << 4)

#define TIM_CCMR2_IC3PSC_OFF 		(0 << 2)
#define TIM_CCMR2_IC3PSC_2			(1 << 2)
#define TIM_CCMR2_IC3PSC_4			(2 << 2)
#define TIM_CCMR2_IC3PSC_8			(3 << 2)

/***** Timer CCER Regs ***********************************************/
#define TIM_CCER_CC4NP 				(1 << 15)
#define TIM_CCER_CC4P 				(1 << 13)
#define TIM_CCER_CC4E 				(1 << 12)
#define TIM_CCER_CC3NP 				(1 << 11)
#define TIM_CCER_CC3P 				(1 << 9)
#define TIM_CCER_CC3E 				(1 << 8)
#define TIM_CCER_CC2NP 				(1 << 7)
#define TIM_CCER_CC2P 				(1 << 5)
#define TIM_CCER_CC2E 				(1 << 4)
#define TIM_CCER_CC1NP 				(1 << 3)
#define TIM_CCER_CC1P 				(1 << 1)
#define TIM_CCER_CC1E 				(1 << 0)



/***** Variables and Function Prototypes ******************************/
volatile uint16_t counter;

void TIM3_delay_ms_init();
void TIM4_init();

#endif