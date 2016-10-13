/** @file sources_command_callbacks.h
 *  @brief Header file for the simple sources LSCP command callback implementation
 *    
 *  This module acts as an interface with the LSCP library by declaring an external 
 *  instance of the command_callback_keys[]. LSCP Library invokes the callbacks
 *  that the application defines in command_callback_keys[].
 *  
 *  The actual implementation of command_callback_keys[] exists in command_settings_callbacks.cpp
 *  
 *  @author Adam Porsch
 *  @bug No known bugs.
 */


#ifndef SOURCES_COMMAND_CALLBACKS_H_
#define SOURCES_COMMAND_CALLBACKS_H_

#include "LSCP_service.h"				//needed since setting_message_callback_keys_type is defined in LSCP_service.h

extern const command_message_callback_keys_type command_callback_keys[];

#define NUM_COMMAND_KEYS		10		//the number of unique commands, remote and local, this application implements

//#defines for Command String Names used throughout the application code
//The "COMMAND_STRING" prefix is used so they will show up grouped in the auto-complete dropdown
#define COMMAND_STRING_CALPGM				"CalPgm"
#define COMMAND_STRING_VERSIONINFO			"VersionInfo"
#define COMMAND_STRING_READEEPROM			"ReadEEPROM"
#define COMMAND_STRING_INPUT_MICRO_STATUS	"InputMicroStatus"
#define COMMAND_STRING_SYNC                 "Sync"
#define COMMAND_STRING_START                "Start"
#define COMMAND_STRING_HEARTBEAT			"<3"
#define COMMAND_STRING_READMEM              "ReadMem"
#define COMMAND_STRING_SETTINGS_POWERON     "SettingsPowerOn"
#define COMMAND_QUERY_INSTRUMENT_INFO		"QueryInstrumentInfo"

/*
 *The following are function prototypes needed by the application to specifically handle remote command responses.
 *In the simple sources, it's not likely the embedded ARM will issue a command to the Android board and expect a response.
 *However, the LSCP library can be used in applications where there exists a "main" micro and n number of "input" micros.
 *In that scenario, the main micro would typically act as the client and send commands to the input micros and expect a
 *response. 
 *
 *Because the LSCP library can't pass a generic data type determined at run-time back up to the application, the
 *job of the remote response message callback is to deposit the response into static memory, defined in 
 *sources_command_callbacks.cpp. The application then sees that the remote command response message has been received, 
 *using message ID to confirm this, and then issues a call to the respective "get" function to retrieve the 
 *remote command message response. The functions below are defined as a way for the application to retrieve the expected 
 *return data after the remote command response message callback stores it in static memory.
 *
 *An example of how this works is the check_status_of_input_micro() function. This command was simply made up to 
 *illustrate how this sequence could work on a more complicated instrument.
 */
uint32_t get_input_micro_status_return_value(void);


#endif /* SOURCES_COMMAND_CALLBACKS_H_ */