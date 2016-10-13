/** @file android_comm_interface_manager.h
 *  @brief the application layer interface needed to wire up the LSCP library and the low level circular buffer service
 *  
 *  This module contains the function prototypes needed to interface
 *  the simple sources application code to the statically declared 
 *  instances of the LSCP library and the low level circular buffer service.
 *  
 *  @author Adam Porsch
 *  @bug No known bugs.
 */

#ifndef ANDROID_COMM_INTERFACE_MANAGER_H_
#define ANDROID_COMM_INTERFACE_MANAGER_H_

#include "sam.h"

/**
 * @brief initialize and wire up the LSCP library to the low level circular buffer service
 * 
 * Called from the simple sources initialization routine
 * 
 * @param none
 * 
 * @return void none
 */
void init_android_comm_interface(void);

/**
 * @brief wrapper function for the LSCP run_packet_reception_and_message_processing_state_machine function
 * 
 * Since the LSCP service is statically instantiated inside the android_comm_interface_manager module, this
 * function allows the simple sources application code to make the necessary calls to 
 * run_packet_reception_and_message_processing_state_machine, which is a member function of the LSCP_service class.
 * 
 * @param none
 * 
 * @return void none
 */
void execute_android_comm_packet_reception_state_machine(void);

/**
 * @brief allows the simple sources application code to issue an outgoing local setting message
 * 
 * This function is a wrapper around the LSCP_service member function generate_and_transmit_setting_message().
 * 
 * This function is typically called from setting related functions as defined in settings_manager.cpp
 * 
 * @param setting_name string literal representing the name of the setting that needs to be transmitted
 * 
 * @return void
 */
void generate_local_setting_message(const char *setting_name);

/**
 * @brief allows the simple sources application code to issue an outgoing remote command message and wait for a response
 * 
 * This function will return from execution under the following two conditions:
 * 1) A remote command response message has been received and the message IDs match
 * 2) The receiver did not return and a timeout has occurred. The sources application code is responsible for
 *    integrating timer timeout functionality into the implementation of this function.
 *    
 * In addition, once this function returns, its up to the calling function, as defined in command_manager, to go and
 * retrieve the returned response value. An example of this is the check_status_of_input_micro() function
 * defined in command_manager.cpp
 * 
 * @param command_name string literal representing the name of the command that needs to be transmitted
 * @param command_data_param command specific data parameters that will be used to generate the data field in the outgoing remote command message * 
 *
 * command_data_param is a void pointer since the application may have a variety of data types that could be used to generate
 * the command parameters specified in the data field of an outgoing remote command message.
 * This field can be NULL if the outgoing remote command message does not require a data parameter
 * 
 * @return uint32_t 1 = timeout, the receiver didn't issue a response, 0 = response received
 */
uint32_t generate_remote_command_message_and_wait_for_response(const char *command_name, void *command_data_param);


#endif /* ANDROID_COMM_INTERFACE_MANAGER_H_ */