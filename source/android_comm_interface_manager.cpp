/** @file android_comm_interface_manager.cpp
 *  @brief module used to wire up serial_circular_buffer and LSCP_service together
 *  
 *  This module contains the statically declared instance of both the
 *  serial_circular_buffer and the LSCP_service to allow them to be wired
 *  up together.
 *  
 *  This module also contains the statically declared circular buffers and LSCP message
 *  buffers.
 *  
 *  @author Adam Porsch
 *  @bug No known bugs.
 */	
#include "android_comm_interface_manager.h"
#include "LSCP_service.h"
#include "serial_circular_buffer_service.h"
#include "sources_command_callbacks.h"
#include "sources_settings_callbacks.h"

//buffers and packet sizes are defined here in application to meet the application requirements
#define ANDROID_TX_UART_BUFFER_SIZE			2048
#define ANDROID_RX_UART_BUFFER_SIZE			2048
#define LSCP_DEFAULT_MAX_MESSAGE_SIZE		512

//the following buffers are the circular buffers used by the instance of the serial_circular_buffer class
char android_uart_Rx_buffer[ANDROID_RX_UART_BUFFER_SIZE];
char android_uart_Tx_buffer[ANDROID_TX_UART_BUFFER_SIZE];

//the following buffers are used by the LSCP_service to store incoming and outgoing LSCP packets, one at a time
char LSCP_rx_message_buffer[LSCP_DEFAULT_MAX_MESSAGE_SIZE];
char LSCP_tx_message_buffer[LSCP_DEFAULT_MAX_MESSAGE_SIZE];

serial_circular_buffer mySerialCircularBuffer;
LSCP_service myLSCPService;


//TODO: REMOVE settings_test_string[]
char settings_test_string[] = "{\"type\":0,\"name\":\"InputASetup\",\"data\":{\"reversing\": true,\"range\": 5,\"Gain\":1.00123,\"Zero\":0.00321},\"id\": 5}";

void init_android_comm_interface(void)
{	
	mySerialCircularBuffer.init(UART_PORT_0,
								android_uart_Rx_buffer,
								ANDROID_RX_UART_BUFFER_SIZE,
								android_uart_Tx_buffer,
								ANDROID_TX_UART_BUFFER_SIZE,
								115200,
								UART_PARITY_NONE);

	myLSCPService.init(&mySerialCircularBuffer, 
					   LSCP_rx_message_buffer, 
					   LSCP_tx_message_buffer, 
					   setting_callback_keys, 
					   NUM_SETTING_KEYS, 
					   command_callback_keys, 
					   NUM_COMMAND_KEYS);
}


void execute_android_comm_packet_reception_state_machine(void)
{	
	myLSCPService.run_packet_reception_and_message_processing_state_machine();
}

void generate_local_setting_message(const char *setting_name)
{
	myLSCPService.generate_and_transmit_setting_message(setting_name, NO_RESPONSE_REQUIRED);
}

uint32_t generate_remote_command_message_and_wait_for_response(const char *command_name, void *command_data_param)
{
	uint32_t id_field;
	uint32_t response_timeout = 0;
	uint32_t timeout_ticker = 0;			//TODO: TEMPORARY, REMOVE
	
	id_field = myLSCPService.generate_and_transmit_remote_command_message(command_name, RESPONSE_REQUIRED, command_data_param);
	
	while(myLSCPService.wait_for_message_response(id_field))
	{
		timeout_ticker++;					//TODO: TEMPORARY, REMOVE. HACK AS PLACEHOLDER FOR APPLICATION TO EVENTUALLY WIRE UP TIMER
		if(timeout_ticker == 3000000)		//Comes out to around 7.5 seconds @ 120MHz
		{
			response_timeout = 1;
			break;
		}
	}
	
	return(response_timeout);
	
}