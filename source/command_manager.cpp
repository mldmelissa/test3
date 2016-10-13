/** @file command_manager.cpp
 *  @brief Implementation file to carry out the simple source commands received over the remote interface
 *    
 *  This module is responsible for defining the functions used to execute commands received over the remote interface. 
 *  These functions are only called by the local_command_and_associated_response_msg_cb as defined in
 *  sources_command_callbacks in order to carry out the execution of a particular command.
 *  
 *  In addition, this module contains functions defined by the application to issue a remote command, wait for a response, 
 *  and then consume that response. The present example of this is check_status_of_input_micro(). These types
 *  of functions are only called by the simple sources application code. *   
 *  
 *  @author Adam Porsch
 *  @bug No known bugs.
 */

#include "command_manager.h"
#include "sources_command_callbacks.h"
#include "android_comm_interface_manager.h"

//TODO: remove, this char array is only here for example purposes of how to handle the execute_VersionInfo_command() function. 
const char *temporary_version_info[] = {"LSA1236", "155", __DATE__ __TIME__ , "1"};  

#pragma region "read EEPROM command support functions"
uint8_t execute_ReadEEPROM_command(uint32_t address)
{
	//TODO: call low level routine defined elsewhere in application that reads a byte from EEPROM
	//returned_byte = read_byte_from_EEPROM(address);
	
	//for now, we'll just return a byte in code space pointed to by the address 
	return(*(uint8_t *)address);
}
#pragma endregion "read EEPROM command support functions"

#pragma region "local VersionInfo command support functions"
const char **execute_VersionInfo_command(void)
{
	//Eventually, this command will call various other functions in the application to
	//retrieve the data/strings needed to populate the version info command response.
	//For now, just return the pointer to the string array that contains the data for this application example
	
	return(temporary_version_info);
}
#pragma endregion "local VersionInfo command support functions"

#pragma region "input micro status support functions"
uint32_t check_status_of_input_micro(uint32_t input_micro_number)
{
	generate_remote_command_message_and_wait_for_response(COMMAND_STRING_INPUT_MICRO_STATUS, (void *)&input_micro_number);
	return(get_input_micro_status_return_value());
}
#pragma endregion "input micro status support functions"