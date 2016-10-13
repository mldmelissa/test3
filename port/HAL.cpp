/*
 * HAL.cpp
 *
 * Created: 8/5/2015 8:30:05 AM
 *  Author: adam.porsch
 */ 

#include "HAL.h"

void init_processor() 
{
	// Call cmsis setup function for clocks, etc.
	SystemInit();
	
	// Configure watchdog
	#ifdef DEBUG
		DISABLE_WATCHDOG();
	#else
		SET_WATCHDOG_TIME(5000);
	#endif
	
	// Configure hardware floating point
	SCB->CPACR |= 0xF << 20;
	
	// Enable all peripheral clocks
	PMC->PMC_PCER0 = 0xFFFFFFFF;
	PMC->PMC_PCER1 = 0xFFFFFFFF;
}

void init_timers() {
    TC1->TC_CHANNEL[0].TC_CMR = TC_CMR_WAVSEL_UP_RC |
                                TC_CMR_WAVE |
                                TC_CMR_ACPC_SET |
                                TC_CMR_ACPA_CLEAR;                      // Waveform mode with reset on register C, register C sets output, register A clears output
    TC1->TC_CHANNEL[0].TC_EMR = TC_EMR_NODIVCLK;                        // Use peripheral clock with no divisor
    UPDATE_SAMPLE_TIMER_FREQUENCY(OUTPUT_SAMPLING_FREQUENCY);           // Set sampling frequency. The frequency represents the rate the DAC is updated with a new output waveform value.
    UPDATE_SAMPLE_TIMER_DUTY(OUTPUT_SAMPLING_FREQUENCY, 0.98);          // Set duty cycle. Used to determine the amount of time LDAC line is low. Example: 600kHz sample f with 98% dty cycle = LDAC low for ~33.33nS.
    NVIC_EnableIRQ(TC3_IRQn);                                           // Enable the corresponding interrupt line in NVIC
    NVIC_SetPriority(TC3_IRQn, 0);                                      // Set priority of interrupt
    DISABLE_TIMER_INTERRUPT();                                          // Disable Interrupt on RC compare until needed by AC mode
    DISABLE_TIMER_CLOCK();                                              // Keep clock disabled until needed for DC mode
}

void init_SPI() {
    // Hall SPI
    SPI->SPI_CR = SPI_CR_SPIEN;                                   // Enable SPI Module
    SPI->SPI_MR = SPI_MR_MODFDIS | SPI_MR_MSTR | SPI_MR_PCS(0);   // Select master mode, use configuration from chip select 0.
    SPI->SPI_CSR[0] = SPI_CSR_BITS_8_BIT;                         // Capture on rising edges, 8 bits
    SET_SPI_BAUD(DAC_SPI_BAUD_RATE);
    PIOA->PIO_PDR = PIO_PDR_P11;                                  // Enable MISO pin to function as peripheral
    PIOA->PIO_ABCDSR[0] &= ~PIO_ABCDSR_P11;                       // Connect peripheral to pin (NPCS0 is peripheral A for pin 11 on port A)
    PIOA->PIO_ABCDSR[1] &= ~PIO_ABCDSR_P11;
    PIOA->PIO_PDR = PIO_PDR_P12;                                  // Enable MISO pin to function as peripheral
    PIOA->PIO_ABCDSR[0] &= ~PIO_ABCDSR_P12;                       // Connect peripheral to pin (MISO is peripheral A for pin 12 on port A)
    PIOA->PIO_ABCDSR[1] &= ~PIO_ABCDSR_P12;
    PIOA->PIO_PDR = PIO_PDR_P13;                                  // Enable MOSI pin to function as peripheral
    PIOA->PIO_ABCDSR[0] &= ~PIO_ABCDSR_P13;                       // Connect peripheral to pin (MOSI is peripheral A for pin 13 on port A)
    PIOA->PIO_ABCDSR[1] &= ~PIO_ABCDSR_P13;
    PIOA->PIO_PDR = PIO_PDR_P14;                                  // Enable clock pin to function as peripheral
    PIOA->PIO_ABCDSR[0] &= ~PIO_ABCDSR_P14;                       // Connect peripheral to pin (SPCK is peripheral A for pin 14 on port A)
    PIOA->PIO_ABCDSR[1] &= ~PIO_ABCDSR_P14;
    
    // Enable the PDC for this channel
    ENABLE_SPI_PDC();
    PDC_SPI->PERIPH_TNCR = 0;
    
    // Enable write protection
    SPI->SPI_WPMR = SPI_WPMR_WPKEY(SPI_WPMR_WPKEY_PASSWD) | SPI_WPMR_WPEN;
    USART0->US_WPMR = US_WPMR_WPKEY(US_WPMR_WPKEY_PASSWD) | US_WPMR_WPEN;
}

/*
 * Initialize only the PIO controlled GPIO lines.
 * IO lines that are tied to peripherals are configured in the respective peripherals init function.
 *
 * At power on reset, PIO lines are, by default, configured as high Z INPUTS with a 100kOhm pull-up enabled.
 * Therefore, the IO lines do not require being actively configured as an input.
 */
void init_gpio() 
{ 
    //enable PIO controller access to pin (as opposed to peripheral) - grouped by PIO blocks
    PIOC->PIO_PER =     DIAG_MON_MUX_0_BIT              |
                        DIAG_MON_MUX_1_BIT              |
                        DIAG_MON_MUX_2_BIT              |
                        DIAG_MON_MUX_3_BIT;
                        
    PIOD->PIO_PER =     LV_VRNG_0_BIT                   |       
                        LV_VRNG_1_BIT                   |
                        LV_OUTMODE_SEL_BIT              |
                        LV_DAC_IN_BIT                   |
                        LV_IRNG_100MA_AC_BYPASS_BIT     |
                        LV_IRNG_10MA_BIT                |
                        LV_IRNG_1MA_BIT                 |
                        LV_IRNG_100UA_BIT               |
                        LV_IRNG_10UA_BIT                |
                        LV_IRNG_1UA_BIT                 |
                        LV_OUTPUT_SHUNT_BIT             |
                        HV_OUTMODE_SEL_BIT              |
                        HV_DAC_IN_BIT                   |
                        HV_IRNG_AC_BYPASS_BIT           |
                        HV_IRNG_10MA_BIT                |
                        HV_IRNG_1MA_BIT                 | 
                        HV_IRNG_100UA_BIT               | 
                        HV_IRNG_10UA_BIT                | 
                        HV_IRNG_1UA_BIT                 |
                        HV_OUTPUT_SHUNT_BIT             |    
                        OUTSTG_SEL_LV_BIT               |
                        OUTSTG_SEL_HV_BIT               |
                        FRONT_REAR_TERM_BIT             |
                        HV_INTERLOCK_MON_BIT; 
                        
    PIOE->PIO_PER =     BOARD_REV_IO_BIT0               |
                        BOARD_REV_IO_BIT1               |
                        BOARD_REV_IO_BIT2               |
                        DEBUG1_IO_BIT                   |
                        DEBUG2_IO_BIT                   |
                        EEPROM_WP_BIT;
                    
    //set initial state of these OUTPUT pins to high - grouped by PIO blocks
    PIOC->PIO_SODR =    DIAG_MON_MUX_0_BIT              |
                        DIAG_MON_MUX_1_BIT              |
                        DIAG_MON_MUX_2_BIT              |
                        DIAG_MON_MUX_3_BIT;
                        
    PIOD->PIO_SODR =    LV_VRNG_0_BIT                   |       
                        LV_VRNG_1_BIT                   | 
                        LV_OUTMODE_SEL_BIT              |
                        LV_DAC_IN_BIT                   |
                        LV_IRNG_100MA_AC_BYPASS_BIT     |
                        LV_IRNG_10MA_BIT                |
                        LV_IRNG_1MA_BIT                 |
                        LV_IRNG_100UA_BIT               |
                        LV_IRNG_10UA_BIT                |
                        LV_IRNG_1UA_BIT                 |
                        LV_OUTPUT_SHUNT_BIT             |
                        HV_OUTMODE_SEL_BIT              |
                        HV_DAC_IN_BIT                   |
                        HV_IRNG_AC_BYPASS_BIT           |
                        HV_IRNG_10MA_BIT                |
                        HV_IRNG_1MA_BIT                 | 
                        HV_IRNG_100UA_BIT               | 
                        HV_IRNG_10UA_BIT                | 
                        HV_IRNG_1UA_BIT                 |
                        HV_OUTPUT_SHUNT_BIT             |
                        OUTSTG_SEL_LV_BIT               |
                        OUTSTG_SEL_HV_BIT               |
                        FRONT_REAR_TERM_BIT;       
                        
    PIOE->PIO_SODR =    DEBUG1_IO_BIT                   |
                        DEBUG2_IO_BIT;           

    //set initial state of these OUTPUT pins to low - grouped by PIO blocks
    PIOE->PIO_CODR =    PIO_CODR_P23;                     
    
    //allow for writes to ODSR register. Needed for synchronous pin level changes     - grouped by PIO blocks                       
    PIOC->PIO_OWER =    DIAG_MON_MUX_0_BIT              |
                        DIAG_MON_MUX_1_BIT              |
                        DIAG_MON_MUX_2_BIT              |
                        DIAG_MON_MUX_3_BIT;
    
    PIOD->PIO_OWER =    LV_VRNG_0_BIT                   |
                        LV_VRNG_1_BIT                   |
                        LV_OUTMODE_SEL_BIT              |
                        LV_DAC_IN_BIT      |
                        LV_IRNG_100MA_AC_BYPASS_BIT     |
                        LV_IRNG_10MA_BIT                |
                        LV_IRNG_1MA_BIT                 |
                        LV_IRNG_100UA_BIT               |
                        LV_IRNG_10UA_BIT                |
                        LV_IRNG_1UA_BIT                 |
                        LV_OUTPUT_SHUNT_BIT             |
                        HV_OUTMODE_SEL_BIT              |
                        HV_DAC_IN_BIT                   |
                        HV_IRNG_AC_BYPASS_BIT           |
                        HV_IRNG_10MA_BIT                |
                        HV_IRNG_1MA_BIT                 |
                        HV_IRNG_100UA_BIT               |
                        HV_IRNG_10UA_BIT                |
                        HV_IRNG_1UA_BIT                 |
                        HV_OUTPUT_SHUNT_BIT             |
                        OUTSTG_SEL_LV_BIT               |
                        OUTSTG_SEL_HV_BIT               |
                        FRONT_REAR_TERM_BIT;  
    
    //enable the pull downs and disable the pull ups on the board rev ID lines so they read properly
    PIOE->PIO_PUDR =    BOARD_REV_IO_BIT0               |
                        BOARD_REV_IO_BIT1               |
                        BOARD_REV_IO_BIT2;
                        
    PIOE->PIO_PPDER =   BOARD_REV_IO_BIT0               |
                        BOARD_REV_IO_BIT1               |
                        BOARD_REV_IO_BIT2;
    
    //finally, enable the output on the line - grouped by PIO blocks                        
    PIOD->PIO_OER =     LV_VRNG_0_BIT                   |
                        LV_VRNG_1_BIT                   |
                        LV_OUTMODE_SEL_BIT              |
                        LV_DAC_IN_BIT      |
                        LV_IRNG_100MA_AC_BYPASS_BIT     |
                        LV_IRNG_10MA_BIT                |
                        LV_IRNG_1MA_BIT                 |
                        LV_IRNG_100UA_BIT               |
                        LV_IRNG_10UA_BIT                |
                        LV_IRNG_1UA_BIT                 |
                        LV_OUTPUT_SHUNT_BIT             |
                        HV_OUTMODE_SEL_BIT              |
                        HV_DAC_IN_BIT                   |
                        HV_IRNG_AC_BYPASS_BIT           |
                        HV_IRNG_10MA_BIT                |
                        HV_IRNG_1MA_BIT                 |
                        HV_IRNG_100UA_BIT               |
                        HV_IRNG_10UA_BIT                |
                        HV_IRNG_1UA_BIT                 |
                        HV_OUTPUT_SHUNT_BIT             |
                        OUTSTG_SEL_LV_BIT               |
                        OUTSTG_SEL_HV_BIT               |
                        FRONT_REAR_TERM_BIT;
                        
    PIOE->PIO_OER =     DEBUG1_IO_BIT                   |
                        DEBUG2_IO_BIT;
                        
    PIOC->PIO_PDR = PIO_PDR_P23;                                        // Enable output pin to function as peripheral
    PIOC->PIO_ABCDSR[0] |= PIO_ABCDSR_P23;                              // Connect peripheral to pin (TIOA3 is peripheral B for pin 23 on port C)
    PIOC->PIO_ABCDSR[1] &= ~PIO_ABCDSR_P23;
    
    //enable write protection so the PIO config won't get corrupted during runtime
    PIOC->PIO_WPMR = PIO_WPMR_WPKEY(PIO_WPMR_WPKEY_PASSWD) | PIO_WPMR_WPEN;
    PIOD->PIO_WPMR = PIO_WPMR_WPKEY(PIO_WPMR_WPKEY_PASSWD) | PIO_WPMR_WPEN;
    PIOE->PIO_WPMR = PIO_WPMR_WPKEY(PIO_WPMR_WPKEY_PASSWD) | PIO_WPMR_WPEN;
}

void delay_ms(float ms) {
    volatile unsigned int i;
    unsigned int ticks;
    
    // Compute tick count that will result in the desired delay
    ticks = (unsigned int)((ms/1000.0)*(float)SystemCoreClock - 30)/205;
    
    for (i = 0; i < ticks; i++) {
        asm("NOP"); // Keep the compiler from touching this
    }
}


void set_HW_configuration_lines(pio_t PIO_port_base_addr, uint32_t mask, uint32_t desired_state_of_IO_line_bits)
{
    uint32_t current_state_of_IO_line_bits = 0;
    
    current_state_of_IO_line_bits = PIO_port_base_addr->PIO_ODSR;
    current_state_of_IO_line_bits &= ~mask;
    
    PIO_port_base_addr->PIO_ODSR = (current_state_of_IO_line_bits | desired_state_of_IO_line_bits);   
    
}
