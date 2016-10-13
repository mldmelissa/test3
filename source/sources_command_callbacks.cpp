/** @file sources_command_callbacks.cpp
 *  @brief Implementation file for the simple sources LSCP command callbacks
 *    
 *  This module contains the implementation of the callbacks for the simple sources LSCP commands.
 *  
 *  This module, along with the sources_settings_callbacks.cpp, are the only application layer
 *  modules that interact with the cJSON library. This is necessary since the data field of each LSCP message
 *  could have a different definition/signature. Since C/C++ does not have a simple way to pass around
 *  generic data types and determine their type at run-time, these callbacks are responsible
 *  for constructing/deconstructing the LSCP command data fields in question, via the cJSON libraries.
 *  
 *  This module also acts as the glue to the application "command manager", which contains the
 *  application layer functions to execute incoming local commands.
 *  
 *  Finally, this module differs slightly from the sources_settings_callbacks module in that
 *  the programmer also needs to define static variables to store the results of remote command responses
 *  and implement a "get" function so the application can retrieve that data. Further explanation of 
 *  this process can be found in sources_command_callbacks.h.
 *  
 *  @author Adam Porsch
 *  @bug No known bugs.
 */


#include "sam.h"
#include "sources_command_callbacks.h"
#include "command_manager.h"
#include "settings_manager.h"
#include "sources_settings_callbacks.h"				//here to access function to generate data field for info message
#include "utility_functions.h"

#pragma region "static variables used to store the returned values of remote command responses"
//"get functions" need to be built around these static variables so the application can retrieve the remote command response data once the message has arrived
uint32_t input_micro_status;
#pragma endregion "static variables used to store the returned values of remote command responses"

#pragma region "prototypes for callback implementations that are restricted to the scope of this module"

//read EEPROM local command
cJSON* local_command_and_associated_response_msg_cb_read_EEPROM(cJSON *read_EEPROM_incoming_data_field);

//CALPGM local command
cJSON* local_command_and_associated_response_msg_cb_calpgm(cJSON *calpgm_incoming_data_field);

//version Info local command
cJSON* local_command_and_associated_response_msg_cb_version_info(cJSON *version_info_incoming_data_field);

//input micro status remote command
cJSON* remote_command_msg_cb_input_micro_status(void *input_micro_number);
void remote_command_response_msg_cb_input_micro_status(cJSON *input_micro_status_incoming_data_field);

//sync local command
cJSON* local_command_and_associated_response_msg_cb_sync(cJSON *sync_incoming_data_field);

//start local command
cJSON* local_command_and_associated_response_msg_cb_start(cJSON *start_incoming_data_field);

//heartbeat local command
cJSON* local_command_and_associated_response_msg_cb_heartbeat(cJSON *heartbeat_incoming_data_field);

//ReadMem local command
cJSON* local_command_and_associated_response_msg_cb_readmem(cJSON *readmem_incoming_data_field);

//Power On Settings local command
cJSON* local_command_and_associated_response_msg_cb_settings_poweron(cJSON *settings_poweron_incoming_data_field);

//Query Version local command
cJSON* local_command_and_associated_response_msg_cb_query_version_info(cJSON *settings_query_version_incoming_data_field);

#pragma endregion "prototypes for callback implementations that are restricted to the scope of this module"

const command_message_callback_keys_type command_callback_keys[NUM_COMMAND_KEYS] =
{
	{COMMAND_STRING_CALPGM,				&local_command_and_associated_response_msg_cb_calpgm,				NULL,										NULL},
	{COMMAND_STRING_READEEPROM,         &local_command_and_associated_response_msg_cb_read_EEPROM,			NULL,                                       NULL},
	{COMMAND_STRING_VERSIONINFO,        &local_command_and_associated_response_msg_cb_version_info,			NULL,                                       NULL},
	{COMMAND_STRING_INPUT_MICRO_STATUS, NULL,																&remote_command_msg_cb_input_micro_status,  &remote_command_response_msg_cb_input_micro_status},
    {COMMAND_STRING_SYNC,               &local_command_and_associated_response_msg_cb_sync,					NULL,                                       NULL},
    {COMMAND_STRING_START,              &local_command_and_associated_response_msg_cb_start,				NULL,                                       NULL},
	{COMMAND_STRING_HEARTBEAT,			&local_command_and_associated_response_msg_cb_heartbeat,			NULL,										NULL},
    {COMMAND_STRING_READMEM,            &local_command_and_associated_response_msg_cb_readmem,				NULL,                                       NULL},
    {COMMAND_STRING_SETTINGS_POWERON,   &local_command_and_associated_response_msg_cb_settings_poweron,		NULL,                                       NULL},
	{COMMAND_QUERY_INSTRUMENT_INFO,		&local_command_and_associated_response_msg_cb_query_version_info,	NULL,										NULL}
};

/*TODO: Don't forget to consider the scenario where the ARM code functions without .NET board present.
  In this scenario, the application would likely implement a different "local_command_and_associated_response_msg_cb_read_EEPROM" call back 
  that the SCPI library would jump into. However, both the LSCP callback and the SCPI callback would both use the same 
  "execute_ReadEEPROM_command" function. This keeps the protocols divorced from the actual application specific execution of the read EEPROM command.
*/

#pragma region "callback implementations related to the CALPGM command"

cJSON* local_command_and_associated_response_msg_cb_calpgm(cJSON *calpgm_incoming_data_field)
{
	(void)calpgm_incoming_data_field;		//here to silence -Wunused-parameter warning

	jump_into_bootloader_mode();

	return(NULL);							//should never get here, we should be executing bootloader code by this point
}


#pragma endregion "callback implementations related to the CALPGM command"

#pragma region "callback implementations related to the ReadEEPROM command"

/**
 * @brief callback to handle incoming local command message for read EEPROM command
 * 
 * The LSCP library will invoke this callback when the android board has sent down a ReadEEPROM command that 
 * we need to execute and respond to.
 * 
 * @param read_EEPROM_incoming_data_field data field of LSCP ReadEEPROM command message, encoded as a cJSON data struct
 * 
 * The address to be read is encoded inside the cJSON struct, as a 1-D array element.
 * 
 * @return cJSON* the requested EEPROM data, encoded as a cJSON data struct
 * 
 * This return value will be used by the LSCP library to generate the local command response message to send 
 * back to the android board.
 */
cJSON* local_command_and_associated_response_msg_cb_read_EEPROM(cJSON *read_EEPROM_incoming_data_field)
{
	uint32_t eeprom_address = 0;
	uint8_t eeprom_data = 0;
	cJSON *LSCP_data_object;
	
	//extract the integer value from the cJSON data struct
	//we know the data type is an integer number based on the simple sources LSCP protocol definition 
	eeprom_address = (uint32_t)(cJSON_GetArrayItem(read_EEPROM_incoming_data_field,0)->valueint);	
	
	eeprom_data = execute_ReadEEPROM_command(eeprom_address);
	
	//per simple sources protocol definition, ReadEERPOM response data field is a 1D array containing the requested EEPROM data
	LSCP_data_object = cJSON_CreateIntArray((const int*)(&eeprom_data),1);
	
	return(LSCP_data_object);	
}
#pragma endregion "callback implementations related to the ReadEEPROM command"

#pragma region "callback implementations related to the VersionInfo command"
/**
 * @brief callback to handle incoming local command message for the VersionInfo command
 * 
 * The LSCP library will invoke this callback when the android board has sent down a VersionInfo command that
 * we need to execute and respond to.
 * 
 * @param version_info_incoming_data_field not applicable
 * 
 * Not all incoming commands have a data field that the application needs in order to execute the command. 
 * However, the callback signature has to accommodate the incoming data field for those cases.
 * The VersionInfo command doesn't have an incoming data field, therefore it will simply be NULL
 * and not consumed by this callback. In this case, 
 * 
 * @return cJSON* the requested VersionInfo data, encoded as a cJSON data struct
 */
cJSON* local_command_and_associated_response_msg_cb_version_info(cJSON *version_info_incoming_data_field)	
{
	const char** returned_version_info;			//pointer to 2D string array containing sources version info strings
	cJSON *version_info_data_object;
	
    (void)version_info_incoming_data_field;         //here to silence -Wunused-parameter warning
    
	returned_version_info =  execute_VersionInfo_command();
	
	version_info_data_object = cJSON_CreateObject();
	cJSON_AddStringToObject(version_info_data_object, "SerialNumber", returned_version_info[0]);
	cJSON_AddStringToObject(version_info_data_object, "ModelNumber", returned_version_info[1]);
	cJSON_AddStringToObject(version_info_data_object, "FirmwareVersion", returned_version_info[2]);
	cJSON_AddStringToObject(version_info_data_object, "BoardRevision", returned_version_info[3]);
	 
	return(version_info_data_object);	
}
#pragma endregion "callback implementations related to the VersionInfo command"

#pragma region "callback implementations related to the InputMicroStatus command"
//---------------------   not apart of the simple sources design, just here for example purposes -----------------------

/**
 * @brief callback to handle generating the data field for the outgoing input micro status remote command message
 * 
 * The LSCP library will invoke this callback when the embedded ARM is acting as a "main" processor and needs to generate 
 * an outgoing remote InputMicroStatus command to a specific "input" embedded micro. 
 * 
 * @param input_micro_number assuming the input micros are enumerated, this value identifies which micro to send the message to
 * 
 * Note, the data type of the remote command message callback is a void pointer. This is to accommodate any data type/strcuts
 * that the application needs to provide to the LSCP library in order to generate the data field parameters for the outgoing 
 * message. In this example, the input micro number is an integer. However, more complex LSCP remote command messages
 * may have floats, arrays or complex objects as data fields. Therefore, the callback structure accommodates this and puts the 
 * burden on the callback implementation to cast the values appropriately to build up the cJSON object to meet the protocol 
 * definition for the specific command.
 * 
 * @return cJSON* the data field containing the input micro number param for the outgoing input micro stats command, encoded as a cJSON data struct
 */
cJSON* remote_command_msg_cb_input_micro_status(void *input_micro_number)
{
	cJSON *input_micro_status_outgoing_data_field;	
	
	input_micro_status_outgoing_data_field = cJSON_CreateNumber(*(uint32_t*)input_micro_number);
		
	return(input_micro_status_outgoing_data_field);
}

/**
 * @brief callback to handle the incoming remote command message response for the input micro status command
 * 
 * The LSCP library will invoke this callback when the input micro processor has sent a input micro status command response message back to 
 * the main micro. This callback will extract the status value from the cJSON field and save it into a static memory location
 * contained within this module. The application will then retrive that information using the get_input_micro_status_return_value().
 * 
 * @param input_micro_status_incoming_data_field data field of LSCP InputMicroVersion command response message, encoded as a cJSON data struct
 * 
 * @return void
 */
void remote_command_response_msg_cb_input_micro_status(cJSON *input_micro_status_incoming_data_field)
{
	input_micro_status = input_micro_status_incoming_data_field->valueint;	
}

/**
 * @brief returns the status of the input micro that the LSCP remote command response callback just stored into static memory.
 * 
 * The application code calls this function when the generate_remote_command_message_and_wait_for_response()
 * function returns, indicating that the requested input micro status is available in memory and ready to be read 
 * out.
 * 
 * @param none
 * 
 * @return uint32_t input micro status, likely some kind of enumeration or bit encoded value
 */
uint32_t get_input_micro_status_return_value(void)
{
	return(input_micro_status);
}
#pragma endregion "callback implementations related to the InputMicroStatus command"

#pragma region "callback implementations related to the Sync command"

/**
 * @brief callback to handle incoming local command message for Sync command
 * 
 * The LSCP library will invoke this callback when the android board has sent down a Sync command that
 * we need to execute and respond to.
 * 
 * This is a unique command that includes setting messages as part of the message exchange sequence.
 * 
 * Sequence:
 * 1) .NET sends Sync Command message to Embedded ARM
 * 2) Embedded ARM first responds with Info Setting Message
 * 3) Embedded ARM then sends back CalData Setting Message
 * 4) Finally, Embedded ARM issues Sync Command response Message
 * 
 * @param sync_incoming_data_field   not used here, Sync cmd doesn't have an input parameter
 * 
 * @return cJSON*   NULL. Sync command response does not have a data field.
 */
cJSON* local_command_and_associated_response_msg_cb_sync(cJSON *sync_incoming_data_field)
{
    (void)sync_incoming_data_field;             //here to silence -Wunused-parameter warning
    
    generate_local_info_setting_message();
    generate_local_caldata_setting_message();                 
    
    return(NULL);    
}

#pragma endregion "callback implementations related to the sync command"

#pragma region "callback implementations related to the start command"


/**
 * @brief  callback to handle incoming local command message for start command
 * 
 * Place holder for future instruments. Output Enable = TRUE accomplishes the same thing.
 * 
 * @param start_incoming_data_field N/A, no incoming data
 * 
 * @return cJSON* returns nothing
 */
cJSON* local_command_and_associated_response_msg_cb_start(cJSON *start_incoming_data_field)
{
    (void)start_incoming_data_field;		//here to silence -Wunused-parameter warning
	
	execute_start_command();
    
    return(NULL);    
}

#pragma endregion "callback implementations related to the start command"

#pragma region "callback implementations related to the heartbeat command"
/**
 * @brief callback to handle incoming local command message for heartbeat command
 * 
 * The LSCP library will invoke this callback when the android board has sent down a heartbeat command.
 * 
 * @param sync_incoming_data_field   not used here, heartbeat command doesn't have an input parameter
 * 
 * @return cJSON*   NULL. Heartbeat command response does not have a data field.
 */
cJSON* local_command_and_associated_response_msg_cb_heartbeat(cJSON *heartbeat_incoming_data_field)
{
    (void)heartbeat_incoming_data_field;	//here to silence -Wunused-parameter warning
		
	// TODO : handle heartbeat
    
    return(NULL);    
}
#pragma endregion "callback implementations related to the heartbeat command"

#pragma region "callback implementations related to the ReadMem command"

/**
 * @brief callback to handle incoming local command message for ReadMem command
 * 
 * The LSCP library will invoke this callback when the android board has sent down a ReadMem command that 
 * we need to execute and respond to.
 * 
 * @param read_mem_incoming_data_field data field of LSCP ReadMem command message, encoded as a cJSON data struct
 * 
 * The address to be read is encoded inside the cJSON struct, as a 1-D array element.
 * 
 * @return cJSON* the requested 32 bit data from memory, encoded as a cJSON data struct
 * 
 * This return value will be used by the LSCP library to generate the local command response message to send 
 * back to the android board.
 */
cJSON* local_command_and_associated_response_msg_cb_readmem(cJSON *readmem_incoming_data_field)
{
	uint32_t memory_address = 0;
	uint32_t memory_data = 0;
	cJSON *LSCP_data_object;
	
	//extract the integer value from the cJSON data struct
	//we know the data type is an integer number based on the simple sources LSCP protocol definition 
	memory_address = (uint32_t)(readmem_incoming_data_field->valueint);	
	
	memory_data = *((uint32_t *)memory_address);

	LSCP_data_object = cJSON_CreateNumber(memory_data);
	
	return(LSCP_data_object);	
}
#pragma endregion "callback implementations related to the ReadMem command"

#pragma region "callback implementations related to the settings Poweron command"

/**
 * @brief callback to handle incoming local command message for SettingsPowerOn command
 * 
 * The LSCP library will invoke this callback when the android board has sent down a SettingsPowerOn command that 
 * we need to execute. This command simply loads the "settings" data struct with the default power on values.
 * It's original intent was to be used during automated system integration testing to put the main board
 * into a known state. However, it may be applicable during normal operation, TBD.
 * 
 * This command only updates the data struct. It does not physically update the hardware, as that's done with
 * the output enable/disable command.
 * 
 * @param settings_poweron_incoming_data_field  N/A, no incoming parameters
 * 
 * @return cJSON*       N/A, no return value
 */
cJSON* local_command_and_associated_response_msg_cb_settings_poweron(cJSON *settings_poweron_incoming_data_field)
{
	(void)settings_poweron_incoming_data_field;                //here to silence -Wunused-parameter warning
    
    load_settings_struct_with_default_values();
	
	return(NULL);	
}
#pragma endregion "callback implementations related to the settings poweron command"

#pragma region "callback implementations related to the settings Query Version command"


cJSON* local_command_and_associated_response_msg_cb_query_version_info(cJSON *settings_query_version_incoming_data_field)
{
	(void)settings_query_version_incoming_data_field;		//here to silence -Wunused-parameter warning

	return(generate_data_field_for_info_setting_msg_cb());

}
#pragma endregion "callback implementations related to the settings Query Version command"

