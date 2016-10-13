/*
 * TemporaryAppliaction.cpp
 *
 * Created: 10/12/2015 12:38:22 PM
 *  Author: adam.porsch
 */ 

#include "sam.h"
#include "hal.h"                                //TODO: REMOVE!! - here for watchdog pet
#include "android_comm_interface_manager.h"
#include "output_control.h"
#include "settings_manager.h"

void init_all(void);

/**
 * \brief Application entry point.
 *
 * \return Unused (ANSI-C compatibility).
 */

uint32_t crude_ticker = 0;      //TODO: REMOVE THESE
uint32_t toggle = 0;
float test_output_value = -1.0;

int main(void)
{        
    init_all();    
    test_init_function();   //TODO: REMOVE. Needed so cal values get non-garbage data.
        
    while (1) 
    {
        //SET_DEBUG1_OUTPUT;
        
		PET_WATCHDOG();
		
		execute_android_comm_packet_reception_state_machine();      
        
        crude_ticker++;             //TODO: REMOVE THIS

        if(crude_ticker > 500000)
        {
            crude_ticker = 0;			
            if(toggle)
            {
                toggle = 0;   
                SET_DEBUG1_OUTPUT;
            }
            else
            {
                toggle = 1;
                CLEAR_DEBUG1_OUTPUT;                
            }

            //update_DAC_output_while_in_DC_mode(test_output_value, 10.0);
            //test_output_value = test_output_value - 1.0;
            //if(test_output_value <= -10.0)
                //test_output_value = -1.0;
        }            
            
        //CLEAR_DEBUG1_OUTPUT;
		
		/*-
		The possibility of tight, deterministic timing requirements may dictate that generate_LSCP_setting_message("ReadingUpdate") be called from a timer ISR.
		In doing this, the main problem is if we're in the middle of processing incoming LSCP packets in low level execute_android_comm_packet_reception_state_machine() 
		from the main loop and the timer ISR fires, the state machine could be in the middle of deserialize_and_process_LSCP_message(), which could cause fragmentation 
		of the heap based on where we are in execution. Another big problem if the code is further down into the malloc() function itself and gets interrupted,
		it could subsequently corrupt the heap. Also, if it happens to be in the middle of copying the JSON string to the Tx circular buffer 
		and gets interrupted, we'd have a one packet sandwiched in the middle of another packet.
		
		The simplest approach is to disable the reading update timer just before execute_android_comm_packet_reception_state_machine() is called from the main loop. This would allow
		all the mallocs and corresponding frees to be called. In addition, it would allow the packet to be copied to the Tx circular buffer, should the command/setting that is 
		being processed require a response, without interruption. The compromise is that, in this scenario, the reading update packet might be delayed up to 1mS 
		while the execute_android_comm_packet_reception_state_machine() finishes execution before the ISR is serviced. 
		
		However, it should be noted this only occurs when the android board issues a command/setting while we're in the middle of sending back readings. In that scenario, 
		the user is likely changing a range or setting up the instrument anyway.
		
		Therefore, a little jitter in the reading update is allowable. In existing LSCI temp controllers/monitors, the input micro isn't constantly being queried/commanded 
		once its setup and generating reading updates that the user may be performing data collection with.
		*/  
    }
}


void init_all() {
    // Disable interrupts for the course of the initialization
    MASK_ALL_INTERRUPTS();
    
    // Setup the microcontroller itself
    init_processor();
    
    // Delay to let the external supplies settle
    delay_ms(200);
    
    // Setup all modules
    init_gpio();
    init_timers();
    init_SPI();    
    init_android_comm_interface();	    
    initialize_output_control_parameters();
    load_settings_struct_with_default_values();
    init_AD5791_DAC();
    
    // Enable interrupts
    UNMASK_INTERRUPTS();
}