/*
 * utility_functions.cpp
 *
 * Created: 9/12/2016 11:18:24 AM
 *  Author: Adam.Porsch
 */ 

 #include "hal.h"

void jump_into_bootloader_mode(void)
{

	CONFIG_MICRO_TO_BOOT_FROM_BOOTLOADER_ROM;

	while(IS_FLASH_CONTROLLER_BUSY_PROCESSING_COMMAND);

	//240 code included this delay to "purge" the UART. I don't think this is necessary as my next line of code resets the micro AND all peripherals.
	//Included in this is resetting the UART used by the ROM bootloader, so I shouldn't have any issues with garbage in the UART shift register when the first bootload command arrives
	//delay_ms(200);

	RESET_MICROCONTROLLER;					//Throw the switch!!

}