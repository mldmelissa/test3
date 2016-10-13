/*
 * HAL.h
 *
 * Created: 8/5/2015 8:30:15 AM
 *  Author: adam.porsch
 */ 


#ifndef HAL_H_
#define HAL_H_

#include "sam.h"

//typedef Uart* uart_t;
typedef Pio*  pio_t;

#define OUTPUT_SAMPLING_FREQUENCY   600e3			//Hz
#define DAC_SPI_BAUD_RATE           25e6			//Hz

// System
#define SET_WATCHDOG_TIME(milliseconds) (WDT->WDT_MR = WDT_MR_WDV((32768*(milliseconds))/(1000*128)) | WDT_MR_WDRSTEN | WDT_MR_WDDBGHLT | WDT_MR_WDIDLEHLT)
#define DISABLE_WATCHDOG() (WDT->WDT_MR = WDT_MR_WDDIS)
//#define PET_WATCHDOG() (WDT->WDT_CR = WDT_CR_KEY_PASSWD | WDT_CR_WDRSTT)
#define PET_WATCHDOG() (WDT->WDT_CR = WDT_CR_KEY(0xA5) | WDT_CR_WDRSTT)			//CMSIS w Atmel Studio 7 changed wdt.h. Key no longer hard coded in wdt.h
#define MASK_ALL_INTERRUPTS() (__disable_irq())
#define UNMASK_INTERRUPTS() (__enable_irq())
void delay_ms(float ms);

//Reset Controller
#define RSTC_CR_KEY_PASSWD					(0xA5u << 24)
#define RESET_MICROCONTROLLER				(RSTC->RSTC_CR = RSTC_CR_KEY_PASSWD | RSTC_CR_PROCRST | RSTC_CR_PERRST)

//Embedded Flash Controller
#define CONFIG_MICRO_TO_BOOT_FROM_BOOTLOADER_ROM		(EFC->EEFC_FCR = EEFC_FCR_FKEY_PASSWD | EEFC_FCR_FARG(1) | EEFC_FCR_FCMD_CGPB)     //ARG of '1' represents 'boot mode selector bit' (GPNVM bit # 1)
#define IS_FLASH_CONTROLLER_BUSY_PROCESSING_COMMAND		(!(EFC->EEFC_FSR & EEFC_FSR_FRDY))


// Timers
#define OUTPUT_UPDATE_TIMER_ISR TC3_Handler
#define UPDATE_SAMPLE_TIMER_FREQUENCY(frequency)    (TC1->TC_CHANNEL[0].TC_RC = TC_RC_RC((uint32_t)(SystemCoreClock/(frequency))))
#define UPDATE_SAMPLE_TIMER_DUTY(frequency, duty)   (TC1->TC_CHANNEL[0].TC_RA = TC_RA_RA((uint32_t)((float)(duty)*(SystemCoreClock/(frequency)))))
#define CLEAR_OUTPUT_UPDATE_TIMER_FLAG()            (TC1->TC_CHANNEL[0].TC_SR)
#define ENABLE_TIMER_CLOCK()                        (TC1->TC_CHANNEL[0].TC_CCR = TC_CCR_CLKEN)
#define DISABLE_TIMER_CLOCK()                       (TC1->TC_CHANNEL[0].TC_CCR = TC_CCR_CLKDIS)
#define ENABLE_TIMER_INTERRUPT()                    (TC1->TC_CHANNEL[0].TC_IER = TC_IER_CPCS)
#define DISABLE_TIMER_INTERRUPT()                   (TC1->TC_CHANNEL[0].TC_IDR = TC_IDR_CPCS)
#define ENABLE_TIMER_CLOCK_DISABLE_ON_RC_COMPARE()  (TC1->TC_CHANNEL[0].TC_CMR |= TC_CMR_CPCDIS)
#define DISABLE_TIMER_CLOCK_DISABLE_ON_RC_COMPARE() (TC1->TC_CHANNEL[0].TC_CMR &= ~TC_CMR_CPCDIS)
#define ISSUE_TIMER_SW_TRIGGER()                    (TC1->TC_CHANNEL[0].TC_CCR = TC_CCR_SWTRG)
#define HAS_RC_COMPARED_SINCE_LAST_STAUS_REG_READ() (TC1->TC_CHANNEL[0].TC_SR & TC_SR_CPCS)

// SPI
#define SPI_ISR SPI_Handler
#define SET_SPI_BAUD(frequency) (SPI->SPI_CSR[0] |= SPI_CSR_SCBR((uint32_t)(SystemCoreClock/(frequency))))
#define SPI_WPMR_WPKEY_PASSWD 0x535049u                                         //CMSIS w Atmel Studio 7 changed spi.h. Key no longer hard coded in spi.h

#define ENABLE_SPI_PDC() (PDC_SPI->PERIPH_PTCR = PERIPH_PTCR_TXTEN)
#define DISABLE_SPI_PDC() (PDC_SPI->PERIPH_PTCR = PERIPH_PTCR_TXTDIS)
#define SET_SPI_PDC_TX_POINTER(address) (PDC_SPI->PERIPH_TPR = (uint32_t)(address))
#define SET_SPI_PDC_TX_COUNT(count) (PDC_SPI->PERIPH_TCR = (count))
#define IS_SPI_PDC_TXBUFFER_EMPTY() (SPI->SPI_SR & SPI_SR_TXBUFE)

//USART
#define US_WPMR_WPKEY_PASSWD 0x555341u

// GPIO - General
#define PIO_WPMR_WPKEY_PASSWD 0x50494Fu
//#define DEBUG_LED_ON() (PIOD->PIO_SODR = PIO_SODR_P23)
//#define DEBUG_LED_OFF() (PIOD->PIO_CODR = PIO_CODR_P23)



//Use this if call to set_HW_configuration_lines() requires IO lines to be changed across functional grouping
#define IO_PORT                     PIOD

//-------------------- GPIO - Low Voltage Output Stage Configuration Lines --------------------

//Port -
#define LV_IO_PORT                  PIOD

//Voltage Range Select
#define LV_VRNG_0_BIT               PIO_ODSR_P0
#define LV_VRNG_1_BIT               PIO_ODSR_P1
#define LV_VRNG_BIT_SHIFT           0
#define LV_VRNG_MASK                ((uint32_t)(LV_VRNG_0_BIT | LV_VRNG_1_BIT))
#define LV_VRNG(n)                  (((n) << LV_VRNG_BIT_SHIFT) & LV_VRNG_MASK)                         //macro to use 0-n range value
#define LV_VRNG_10V                 LV_VRNG(0)
#define LV_VRNG_1V                  LV_VRNG(1)
#define LV_VRNG_100MV               LV_VRNG(2)
#define LV_VRNG_10MV                LV_VRNG(3)

//LV Output Functionality Select
#define LV_OUTMODE_SEL_BIT          PIO_ODSR_P2
#define LV_OUTMODE_SEL_BIT_SHIFT    2
#define LV_OUTMODE_SEL_MASK         ((uint32_t) LV_OUTMODE_SEL_BIT)
#define LV_OUTMODE_SEL(n)           (((n) << LV_OUTMODE_SEL_BIT_SHIFT) & LV_OUTMODE_SEL_MASK)
#define LV_OUTMODE_SEL_VOLTAGE      LV_OUTMODE_SEL(0)
#define LV_OUTMODE_SEL_CURRENT      LV_OUTMODE_SEL(1)

//LV DAC Input Disable 
#define LV_DAC_IN_BIT               PIO_ODSR_P3
#define LV_DAC_IN_BIT_SHIFT         3
#define LV_DAC_IN_MASK              ((uint32_t) LV_DAC_IN_BIT)
#define LV_DAC_IN(n)                (((n) << LV_DAC_IN_BIT_SHIFT) & LV_DAC_IN_MASK)               //macro to use 0-n range value
#define LV_DAC_IN_ENABLE            LV_DAC_IN(0)
#define LV_DAC_IN_DISABLE           LV_DAC_IN(1)

//Current Range Select - Lines are active LOW
#define LV_IRNG_100MA_AC_BYPASS_BIT PIO_ODSR_P4
#define LV_IRNG_10MA_BIT            PIO_ODSR_P5
#define LV_IRNG_1MA_BIT             PIO_ODSR_P6
#define LV_IRNG_100UA_BIT           PIO_ODSR_P7
#define LV_IRNG_10UA_BIT            PIO_ODSR_P8
#define LV_IRNG_1UA_BIT             PIO_ODSR_P9
#define LV_IRNG_BIT_SHIFT           4
#define LV_IRNG_MASK                ((uint32_t)(LV_IRNG_100MA_AC_BYPASS_BIT | LV_IRNG_10MA_BIT | LV_IRNG_1MA_BIT | LV_IRNG_100UA_BIT | LV_IRNG_10UA_BIT | LV_IRNG_1UA_BIT))
#define LV_IRNG(n)                  (((n) << LV_IRNG_BIT_SHIFT) & LV_IRNG_MASK)                         //macro to use 0-n range value
#define LV_IRNG_100MA_AC_BYPASS     LV_IRNG(~0x00000001)      
#define LV_IRNG_10MA                LV_IRNG(~0x00000002)
#define LV_IRNG_1MA                 LV_IRNG(~0x00000004)
#define LV_IRNG_100UA               LV_IRNG(~0x00000008)
#define LV_IRNG_10UA                LV_IRNG(~0x00000010)
#define LV_IRNG_1UA                 LV_IRNG(~0x00000020)

//Output Shunt - Line is active LOW
#define LV_OUTPUT_SHUNT_BIT         PIO_ODSR_P10
#define LV_OUTPUT_SHUNT_BIT_SHIFT   10
#define LV_OUTPUT_SHUNT_MASK        ((uint32_t) LV_OUTPUT_SHUNT_BIT)
#define LV_OUTPUT_SHUNT(n)          (((n) << LV_OUTPUT_SHUNT_BIT_SHIFT) & LV_OUTPUT_SHUNT_MASK)
#define LV_OUTPUT_SHUNT_DISABLE     LV_OUTPUT_SHUNT(0)
#define LV_OUTPUT_SHUNT_ENABLE      LV_OUTPUT_SHUNT(1)

//-------------------- GPIO - High Voltage Output Stage Configuration Lines --------------------

//Port
#define HV_IO_PORT                  PIOD

//HV Output Functionality Select
#define HV_OUTMODE_SEL_BIT          PIO_ODSR_P11
#define HV_OUTMODE_SEL_BIT_SHIFT    11
#define HV_OUTMODE_SEL_MASK         ((uint32_t) HV_OUTMODE_SEL_BIT)
#define HV_OUTMODE_SEL(n)           (((n) << HV_OUTMODE_SEL_BIT_SHIFT) & HV_OUTMODE_SEL_MASK)
#define HV_OUTMODE_SEL_VOLTAGE      HV_OUTMODE_SEL(0)
#define HV_OUTMODE_SEL_CURRENT      HV_OUTMODE_SEL(1)

//HV DAC Input Disable
#define HV_DAC_IN_BIT               PIO_ODSR_P12
#define HV_DAC_IN_BIT_SHIFT         12
#define HV_DAC_IN_MASK              ((uint32_t) HV_DAC_IN_BIT)
#define HV_DAC_IN(n)                (((n) << HV_DAC_IN_BIT_SHIFT) & HV_DAC_IN_MASK)               //macro to use 0-n range value
#define HV_DAC_IN_ENABLE            HV_DAC_IN(0)
#define HV_DAC_IN_DISABLE           HV_DAC_IN(1)

//Programmable Limit Bypass when in AC Mode - Line is active LOW
//#define HV_IRNG_AC_BYPASS_BIT       PIO_ODSR_P13
//#define HV_IRNG_AC_BYPASS_BIT_SHIFT 13
//#define HV_IRNG_AC_BYPASS_MASK      ((uint32_t) HV_IRNG_AC_BYPASS_BIT)
//#define HV_IRNG_AC_BYPASS(n)        (((n) << HV_IRNG_AC_BYPASS_BIT_SHIFT) & HV_IRNG_AC_BYPASS_MASK)     //macro to use 0-n range value
//#define HV_IRNG_AC_BYPASS_ENABLE    HV_IRNG_AC_BYPASS(0)
//#define HV_IRNG_AC_BYPASS_DISABLE   HV_IRNG_AC_BYPASS(1)

//Current Range Select - Lines are active LOW

#define HV_IRNG_AC_BYPASS_BIT       PIO_ODSR_P13
#define HV_IRNG_10MA_BIT            PIO_ODSR_P14
#define HV_IRNG_1MA_BIT             PIO_ODSR_P15
#define HV_IRNG_100UA_BIT           PIO_ODSR_P16
#define HV_IRNG_10UA_BIT            PIO_ODSR_P17
#define HV_IRNG_1UA_BIT             PIO_ODSR_P18
#define HV_IRNG_BIT_SHIFT           13
#define HV_IRNG_MASK                ((uint32_t)(HV_IRNG_AC_BYPASS_BIT | HV_IRNG_10MA_BIT | HV_IRNG_1MA_BIT | HV_IRNG_100UA_BIT | HV_IRNG_10UA_BIT | HV_IRNG_1UA_BIT))
#define HV_IRNG(n)                  (((n) << HV_IRNG_BIT_SHIFT) & HV_IRNG_MASK)                         //macro to use 0-n range value
#define HV_IRNG_AC_BYPASS           HV_IRNG(~0x00000001)
#define HV_IRNG_10MA                HV_IRNG(~0x00000002)
#define HV_IRNG_1MA                 HV_IRNG(~0x00000004)
#define HV_IRNG_100UA               HV_IRNG(~0x00000008)
#define HV_IRNG_10UA                HV_IRNG(~0x00000010)
#define HV_IRNG_1UA                 HV_IRNG(~0x00000020)

//Output Shunt - Line is active LOW
#define HV_OUTPUT_SHUNT_BIT         PIO_ODSR_P19
#define HV_OUTPUT_SHUNT_BIT_SHIFT   19
#define HV_OUTPUT_SHUNT_MASK        ((uint32_t) HV_OUTPUT_SHUNT_BIT)
#define HV_OUTPUT_SHUNT(n)          (((n) << HV_OUTPUT_SHUNT_BIT_SHIFT) & HV_OUTPUT_SHUNT_MASK)
#define HV_OUTPUT_SHUNT_DISABLE     HV_OUTPUT_SHUNT(0)
#define HV_OUTPUT_SHUNT_ENABLE      HV_OUTPUT_SHUNT(1)

//-------------------- GPIO - Output Terminal Select Configuration Lines --------------------
//Port
#define OUT_TERMINAL_SEL_IO_PORT    PIOD

//Output Stage Select - Lines are active LOW
#define OUTSTG_SEL_LV_BIT           PIO_ODSR_P20
#define OUTSTG_SEL_HV_BIT           PIO_ODSR_P21
#define OUTSTG_SEL_BIT_SHIFT        20
#define OUTSTG_SEL_MASK             ((uint32_t) (OUTSTG_SEL_LV_BIT | OUTSTG_SEL_HV_BIT))
#define OUTSTG_SEL(n)               (((n) << OUTSTG_SEL_BIT_SHIFT) & OUTSTG_SEL_MASK)
#define OUTSTG_SEL_LV               OUTSTG_SEL(~0x00000001)
#define OUTSTG_SEL_HV               OUTSTG_SEL(~0x00000002)
#define OUTSTG_SEL_OPEN             OUTSTG_SEL(0x00000003)

//Front / Rear Terminal Select - Lines are active LOW
#define FRONT_REAR_TERM_BIT         PIO_ODSR_P22
#define FRONT_REAR_TERM_BIT_SHIFT   22
#define FRONT_REAR_TERM_MASK        ((uint32_t) FRONT_REAR_TERM_BIT)
#define FRONT_REAR_TERM(n)          (((n) << FRONT_REAR_TERM_BIT_SHIFT) & FRONT_REAR_TERM_MASK)
#define FRONT_REAR_TERM_FRONT_SEL   FRONT_REAR_TERM(0)
#define FRONT_REAR_TERM_REAR_SEL    FRONT_REAR_TERM(1)


//-------------------- GPIO - Diagnostic Signal Monitor MUX Configuration lines --------------------
#define DIAG_MON_MUX_IO_PORT        PIOC

//Multiplexer Channel Select Lines
#define DIAG_MON_MUX_0_BIT          PIO_ODSR_P16
#define DIAG_MON_MUX_1_BIT          PIO_ODSR_P17
#define DIAG_MON_MUX_2_BIT          PIO_ODSR_P18
#define DIAG_MON_MUX_3_BIT          PIO_ODSR_P19
#define DIAG_MON_MUX_BIT_SHIFT      16
#define DIAG_MON_MUX_MASK           ((uint32_t)(DIAG_MON_MUX_0_BIT | DIAG_MON_MUX_1_BIT | DIAG_MON_MUX_2_BIT | DIAG_MON_MUX_3_BIT))
#define DIAG_MON_MUX(n)             (((n) << DIAG_MON_MUX_BIT_SHIFT) & DIAG_MON_MUX_MASK)                         //macro to use 0-n range value

#define DIAG_MON_MUX_DAC_OUT        DIAG_MON_MUX(0)
#define DIAG_MON_MUX_FILT_DAC_OUT   DIAG_MON_MUX(1)
#define DIAG_MON_MUX_LV_OUT         DIAG_MON_MUX(2)
#define DIAG_MON_MUX_LV_OVLD        DIAG_MON_MUX(3)
#define DIAG_MON_MUX_HV_OUT         DIAG_MON_MUX(4)
#define DIAG_MON_MUX_HV_OVLD        DIAG_MON_MUX(5)
#define DIAG_MON_MUX_LV_IOUT        DIAG_MON_MUX(6)
#define DIAG_MON_MUX_LV_GUARD       DIAG_MON_MUX(7)
#define DIAG_MON_MUX_HV_IOUT        DIAG_MON_MUX(8)
#define DIAG_MON_MUX_HV_GUARD       DIAG_MON_MUX(9)

//-------------------- GPIO - Miscellaneous Lines --------------------
//Board revision Lines - Inputs
#define BOARD_REV_IO_BIT0           PIO_PDSR_P0             //PDSR macro used simply b/c PDSR reg is used to read inputs
#define BOARD_REV_IO_BIT1           PIO_PDSR_P1
#define BOARD_REV_IO_BIT2           PIO_PDSR_P2
#define BOARD_REV_IO_BIT_SHIFT      0       
#define BOARD_REV_IO_MASK           ((uint32_t) (BOARD_REV_IO_BIT0 | BOARD_REV_IO_BIT1 | BOARD_REV_IO_BIT2))
#define READ_BOARD_REV_ID_VALUE     (((PIOE->PDSR) & BOARD_REV_IO_MASK) >> BOARD_REV_IO_BIT_SHIFT)

//Debug lines - Outputs
#define DEBUG1_IO_BIT               PIO_SODR_P3             //SODR and CODR are mapped the same in the uP register
#define DEBUG2_IO_BIT               PIO_SODR_P4
#define SET_DEBUG1_OUTPUT           PIOE->PIO_SODR = DEBUG1_IO_BIT
#define CLEAR_DEBUG1_OUTPUT         PIOE->PIO_CODR = DEBUG1_IO_BIT
#define SET_DEBUG2_OUTPUT           PIOE->PIO_SODR = DEBUG2_IO_BIT
#define CLEAR_DEBUG2_OUTPUT         PIOE->PIO_CODR = DEBUG2_IO_BIT

//Cal EERPROM Write Protect Bit
#define EEPROM_WP_BIT               PIO_SODR_P5
#define ENABLE_EEPROM_WP            PIOE->PIO_SODR = EEPROM_WP_BIT
#define DISABLE_EEPROM_WP           PIOE->PIO_CODR = EEPROM_WP_BIT

//High Voltage Interlock Monitor - Input
#define HV_INTERLOCK_MON_BIT        PIO_PDSR_P23
#define HV_INTERLOCK_MON_BIT_SHIFT  23
#define HV_INTERLOCK_MON_MASK       ((uint32_t) HV_INTERLOCK_MON_BIT)
#define READ_HV_INTERLOCK_MONITOR   (((PIOD->PDSR) & HV_INTERLOCK_MON_MASK) >> HV_INTERLOCK_MON_BIT_SHIFT)

void init_timers();
void init_SPI();
void init_gpio();
void init_processor();

void set_HW_configuration_lines(pio_t PIO_port_base_addr, uint32_t mask, uint32_t desired_state_of_IO_line_bits);




#endif /* HAL_H_ */