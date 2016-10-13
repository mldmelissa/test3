/** @file sources_settings_callbacks.h
 *  @brief Header file for the simple sources LSCP setting callback implementation
 *    
 *  This module acts as an interface with the LSCP library by declaring an external 
 *  instance of the setting_callback_keys[]. LSCP Library invokes the callbacks
 *  that the application defines in setting_callback_keys[].
 *  
 *  The actual implementation of setting_callback_keys[] exists in sources_settings_callbacks.cpp
 *  
 *  @author Adam Porsch
 *  @bug No known bugs.
 */


#ifndef SOURCES_SETTINGS_CALLBACKS_H_
#define SOURCES_SETTINGS_CALLBACKS_H_

#include "LSCP_service.h"					//needed since setting_message_callback_keys_type is defined in LSCP_service.h

//info setting
cJSON* generate_data_field_for_info_setting_msg_cb(void);			//needed by queryinfo? in source_command_callbacks

extern const setting_message_callback_keys_type setting_callback_keys[];	

#define NUM_SETTING_KEYS		18			//the number of unique settings, remote and local, this application implements

//#defines for Setting String Names used throughout the application code
//The "SETTING_STRING" prefix used so they will show up grouped in the auto-complete dropdown
#define SETTING_STRING_MODE                         "Mode"
#define SETTING_STRING_OUTPUT_STATE                 "OutputState"
#define SETTING_STRING_CALLOCKED                    "CalLocked"
#define SETTING_STRING_FREQUENCY			        "Frequency"
#define SETTING_STRING_SHAPE                        "Shape"
#define SETTING_STRING_VOLTAGE_RANGE		        "VoltageRange"
#define SETTING_STRING_VOLTAGE_AUTORANGE_ENABLED    "VoltageAutorangeEnabled"
#define SETTING_STRING_VOLTAGE_OUTPUT_LEVEL         "VoltageOutputLevel"
#define SETTING_STRING_CURRENT_OUTPUT_LEVEL		    "CurrentOutputLevel"
#define SETTING_STRING_CURRENT_RANGE		        "CurrentRange"
#define SETTING_STRING_CURRENT_AUTORANGE_ENABLED    "CurrentAutoRangeEnabled"
#define SETTING_STRING_CURRENT_COMPLIANCE_RANGE     "CurrentComplianceRange"
#define SETTING_STRING_CURRENT_COMPLIANCE_STATUS    "CurrentCompliance"
#define SETTING_STRING_VOLTAGE_PROTECTION_STATUS    "VoltageProtection"
#define SETTING_STRING_TERMINALS                    "Terminals"
#define SETTING_STRING_INFO                         "Info"
#define SETTING_INPUT_READINGS                      "InputReadings"     //TODO: Remove. This is being implemented to run a throughput/timing/execution test
#define SETTING_STRING_CALDATA                      "CalData"


#endif /* SOURCES_SETTINGS_CALLBACKS_H_ */