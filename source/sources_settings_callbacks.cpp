/** @file sources_settings_callbacks.cpp
 *  @brief Implementation file for the simple sources LSCP setting callbacks
 *    
 *  This module contains the implementation of the callbacks for the simple sources LSCP settings.
 *  
 *  This module, along with the sources_commands_callbacks.cpp, are the only application layer
 *  modules that interact with the cJSON library. This is necessary since the data field of each LSCP message
 *  could have a different definition/signature. Since C/C++ does not have a simple way to pass around
 *  generic data types and determine their type at run-time, these callbacks are responsible
 *  for constructing/deconstructing the LSCP setting data fields in question, via the cJSON libraries.
 *  
 *  This module also acts as the glue to the application "settings manager", which contains the
 *  functions to validate, apply, and retrieve a specific setting.
 *  
 *  @author Adam Porsch
 *  @bug No known bugs.
 */

#include "sam.h"
#include "sources_settings_callbacks.h"
#include "settings_manager.h"
#include "calibration.h"


#pragma region "prototypes for callback implementations that are restricted to the scope of this module"

//mode setting
void local_setting_msg_cb_mode(cJSON *mode_data_object);
cJSON* generate_data_field_for_mode_setting_msg_cb(void);

//output state setting
void local_setting_msg_cb_output_state(cJSON *output_state_data_object);
cJSON* generate_data_field_for_output_state_setting_msg_cb(void);

//CalLocked setting
void local_setting_msg_cb_callocked(cJSON *output_state_data_object);
cJSON* generate_data_field_for_callocked_setting_msg_cb(void);

//frequency setting
void local_setting_msg_cb_frequency(cJSON *frequency_data_object);
cJSON* generate_data_field_for_frequency_setting_msg_cb(void);

//shape setting
void local_setting_msg_cb_shape(cJSON *shape_data_object);
cJSON* generate_data_field_for_shape_setting_msg_cb(void);

//voltage range setting
void local_setting_msg_cb_voltage_range(cJSON *voltage_range_data_object);
cJSON* generate_data_field_for_voltage_range_setting_msg_cb(void);

//voltage autorange setting
void local_setting_msg_cb_voltage_autorange_enabled(cJSON *voltage_autorange_data_object);
cJSON* generate_data_field_for_voltage_autorange_enabled_setting_msg_cb(void);

//voltage level setting
void local_setting_msg_cb_voltage_level(cJSON *voltage_level_data_object);
cJSON* generate_data_field_for_voltage_level_setting_msg_cb(void);

//voltage level setting
void local_setting_msg_cb_current_level(cJSON *current_level_data_object);
cJSON* generate_data_field_for_current_level_setting_msg_cb(void);

//current range setting
void local_setting_msg_cb_current_range(cJSON *current_range_data_object);
cJSON* generate_data_field_for_current_range_setting_msg_cb(void);

//current autorange setting
void local_setting_msg_cb_current_autorange_enabled(cJSON *current_autorange_data_object);
cJSON* generate_data_field_for_current_autorange_enabled_setting_msg_cb(void);

//current compliance range setting
void local_setting_msg_cb_current_compliance_range(cJSON *current_compliance_range_data_object);
cJSON* generate_data_field_for_current_compliance_range_setting_msg_cb(void);

//current compliance status setting
cJSON* generate_data_field_for_current_compliance_status_setting_msg_cb(void);

//voltage protection status setting
cJSON* generate_data_field_for_voltage_protection_setting_msg_cb(void);

//terminals setting
void local_setting_msg_cb_terminals(cJSON *terminals_data_object);
cJSON* generate_data_field_for_terminals_setting_msg_cb(void);

//CalData setting
void local_setting_msg_cb_caldata(cJSON *caldata_data_object);
cJSON* generate_data_field_for_caldata_setting_msg_cb(void);

//input reading
cJSON* generate_data_field_for_input_reading_msg_cb(void);

#pragma endregion "prototypes for callback implementations that are restricted to the scope of this module"



//setting_message_callback_keys_type is defined in LSCP_service.h. 
//We need to conform to the callback signature as defined by the LSCP library
const setting_message_callback_keys_type setting_callback_keys[NUM_SETTING_KEYS] =
{
    {SETTING_STRING_MODE,                       local_setting_msg_cb_mode,                      NULL, generate_data_field_for_mode_setting_msg_cb},
    {SETTING_STRING_OUTPUT_STATE,               local_setting_msg_cb_output_state,              NULL, generate_data_field_for_output_state_setting_msg_cb}, 
    {SETTING_STRING_CALLOCKED,                  local_setting_msg_cb_callocked,                 NULL, generate_data_field_for_callocked_setting_msg_cb},
    {SETTING_STRING_FREQUENCY,                  local_setting_msg_cb_frequency,                 NULL, generate_data_field_for_frequency_setting_msg_cb},
    {SETTING_STRING_SHAPE,                      local_setting_msg_cb_shape,                     NULL, generate_data_field_for_shape_setting_msg_cb},
	{SETTING_STRING_VOLTAGE_RANGE,              local_setting_msg_cb_voltage_range,             NULL, generate_data_field_for_voltage_range_setting_msg_cb},
    {SETTING_STRING_VOLTAGE_AUTORANGE_ENABLED,  local_setting_msg_cb_voltage_autorange_enabled, NULL, generate_data_field_for_voltage_autorange_enabled_setting_msg_cb},
    {SETTING_STRING_VOLTAGE_OUTPUT_LEVEL,       local_setting_msg_cb_voltage_level,             NULL, generate_data_field_for_voltage_level_setting_msg_cb},
    {SETTING_STRING_CURRENT_OUTPUT_LEVEL,       local_setting_msg_cb_current_level,             NULL, generate_data_field_for_current_level_setting_msg_cb},
	{SETTING_STRING_CURRENT_RANGE,              local_setting_msg_cb_current_range,             NULL, generate_data_field_for_current_range_setting_msg_cb}, 
    {SETTING_STRING_CURRENT_AUTORANGE_ENABLED,  local_setting_msg_cb_current_autorange_enabled, NULL, generate_data_field_for_current_autorange_enabled_setting_msg_cb},
    {SETTING_STRING_CURRENT_COMPLIANCE_RANGE,   local_setting_msg_cb_current_compliance_range,  NULL, generate_data_field_for_current_compliance_range_setting_msg_cb},
    {SETTING_STRING_CURRENT_COMPLIANCE_STATUS,  NULL,                                           NULL, generate_data_field_for_current_compliance_status_setting_msg_cb},
    {SETTING_STRING_VOLTAGE_PROTECTION_STATUS,  NULL,                                           NULL, generate_data_field_for_voltage_protection_setting_msg_cb},
    {SETTING_STRING_TERMINALS,                  local_setting_msg_cb_terminals,                 NULL, generate_data_field_for_terminals_setting_msg_cb},
    {SETTING_STRING_INFO,                       NULL,                                           NULL, generate_data_field_for_info_setting_msg_cb},
    {SETTING_STRING_CALDATA,                    local_setting_msg_cb_caldata,                   NULL, generate_data_field_for_caldata_setting_msg_cb},
    {SETTING_INPUT_READINGS,                    NULL,                                           NULL, generate_data_field_for_input_reading_msg_cb}       	
};


/*TODO: Don't forget to consider the scenario where the ARM code functions without .NET board present.
  in this scenario, I would likely have a different "local_setting_msg_cb_voltage_range" call back that the SCPI library jumps into.
  However, both the LSCP callback and the SCPI callback would both use the same "validate_voltage_range" and "set_voltage_range" functions
  since they are dependent on the voltage_range_type, which is completely divorced from the respective protocols.
*/ 

#pragma region "callback implementations related to the mode setting"
/**
 * @brief callback to handle incoming setting message for local mode setting
 * 
 * The LSCP library will invoke this callback when the android board has sent down a mode
 * setting that we need to apply. Modes are defined as either voltage output or current output
 * 
 * @param mode_data_object data field of LSCP mode setting message, encoded as a cJSON data struct
 * 
 * @return void
 */
void local_setting_msg_cb_mode(cJSON *mode_data_object)
{
	int32_t dirty_mode;
	
	//extract the integer value from the cJSON data struct
	//we know the data type is an integer number based on the simple sources LSCP protocol definition
	dirty_mode = mode_data_object->valueint;				
	
	if(simple_validate_setting(dirty_mode, VOLTAGE_MODE, CURRENT_MODE))
	{
		set_mode_setting((output_mode_type)dirty_mode, DO_NOT_NOTIFY_ANDROID_OF_SETTING_CHANGE);	//LSCP library will take care of the response
	}
}

/**
 * @brief callback to generate data field for an outgoing LSCP mode setting message
 * 
 * The LSCP library will invoke this callback for the following scenarios:
 * 1) The application needs to generate a setting response message to the android board, per its request, letting it know what the actual mode is
 *    in response to the mode setting message the android board just sent down.
 * 2) The application needs to initiate the generation of a setting message to tell the android what the present mode is. 
 * 
 * @param none
 * 
 * @return cJSON* a dynamically allocated cJSON struct containing the LSCP mode setting message data field
 */
cJSON* generate_data_field_for_mode_setting_msg_cb(void)
{
	cJSON *LSCP_data_field;
	output_mode_type output_mode;
	
	output_mode = get_working_mode_setting();
	
	LSCP_data_field = cJSON_CreateNumber(output_mode);
		
	return(LSCP_data_field);	
}
#pragma endregion "callback implementations related to the mode setting"

#pragma region "callback implementations related to the output state setting"
/**
 * @brief callback to handle incoming setting message for local output state setting
 * 
 * The LSCP library will invoke this callback when the android board has sent down a new output state
 * setting that we need to apply. This either enables or disables the output terminals
 * 
 * @param output_state_data_object data field of LSCP output state setting message, encoded as a cJSON data struct
 * 
 * @return void
 */
void local_setting_msg_cb_output_state(cJSON *output_state_data_object)
{	
	bool enable_output;    
    
    if((output_state_data_object->type >= cJSON_False) && (output_state_data_object->type < cJSON_NULL)) //check to make sure cJSON type value isn't corrupt
    {
        if(output_state_data_object->type == cJSON_True)
            enable_output = true;
        else
            enable_output = false;          

        set_output_state_enabled_setting(enable_output, DO_NOT_NOTIFY_ANDROID_OF_SETTING_CHANGE);	//LSCP library will take care of the response   
    }
}

/**
 * @brief callback to generate data field for an outgoing LSCP output state setting message
 * 
 *  The LSCP library will invoke this callback when the application needs to generate a setting response message to the android board, 
 *  per its request, letting it know what the output state was set to in response to the output state setting message the android board just sent down.
 *  
 *  It's not anticipated that the application will need to send an unsolicited output state setting message to the android board, as the output state
 *  shouldn't be changing unless instructed by the android board.
 *
 * @param 
 * 
 * @return cJSON* a dynamically allocated cJSON struct containing the LSCP output state setting message data field
 */
cJSON* generate_data_field_for_output_state_setting_msg_cb(void)
{
	cJSON *LSCP_data_field;
	bool output_state_enabled;
	
	output_state_enabled = is_output_state_enabled();
	
	LSCP_data_field = cJSON_CreateBool(output_state_enabled);
	
	return(LSCP_data_field);
}
#pragma endregion "callback implementations related to the output state setting"

#pragma region "callback implementations related to the callocked setting"
/**
 * @brief callback to handle incoming setting message for local callocked setting
 * 
 * The LSCP library will invoke this callback when the android board has sent down a new callocked
 * setting that we need to apply. Firmware will check this flag to determine if it can execute a CALSAVE commmand.
 * 
 * @param callocked_data_object data field of LSCP callocked setting message, encoded as a cJSON data struct
 * 
 * @return void
 */
void local_setting_msg_cb_callocked(cJSON *callocked_data_object)
{	
	bool dirty_calibration_locked;
    
    if((callocked_data_object->type >= cJSON_False) && (callocked_data_object->type < cJSON_NULL)) //check to make sure cJSON type value isn't corrupt
    {
        if(callocked_data_object->type == cJSON_True)
            dirty_calibration_locked = true;
        else
            dirty_calibration_locked = false;

        set_calibration_locked_setting(dirty_calibration_locked, DO_NOT_NOTIFY_ANDROID_OF_SETTING_CHANGE);	//LSCP library will take care of the response
    }
}

/**
 * @brief callback to generate data field for an outgoing LSCP callocked setting message
 * 
 *  The LSCP library will invoke this callback when the application needs to generate a setting response message to the android board, 
 *  per its request, letting it know what the callocked was set to in response to the callocked setting message the android board just sent down.
 *  
 *  It's not anticipated that the application will need to send an unsolicited callocked setting message to the android board, as the callocked
 *  shouldn't be changing unless instructed by the android board.
 *
 * @param 
 * 
 * @return cJSON* a dynamically allocated cJSON struct containing the LSCP callocked setting message data field
 */
cJSON* generate_data_field_for_callocked_setting_msg_cb(void)
{
	cJSON *LSCP_data_field;
	bool calibration_locked;
	
	calibration_locked = is_calibration_locked();
	
	LSCP_data_field = cJSON_CreateBool(calibration_locked);
	
	return(LSCP_data_field);
}
#pragma endregion "callback implementations related to the callocked setting"

#pragma region "callback implementations related to the frequency setting"
/**
 * @brief callback to handle incoming setting message for local frequency setting
 * 
 * The LSCP library will invoke this callback when the android board has sent down a new frequency
 * setting that we need to apply.
 * 
 * @param frequency_data_object data field of LSCP current range setting message, encoded as a cJSON data struct
 * 
 * @return void
 */
void local_setting_msg_cb_frequency(cJSON *frequency_data_object)
{	
	float dirty_frequency;
	
	//extract the double value from the cJSON data struct. cJSON works in doubles, this application works in float to take advantage of the ARM FPU.
	//we know the data type is an floating point number based on the simple sources LSCP protocol definition
	dirty_frequency = (float)frequency_data_object->valuedouble;
	
	if(validate_frequency_setting(dirty_frequency))
	{
		set_frequency_setting(dirty_frequency, DO_NOT_NOTIFY_ANDROID_OF_SETTING_CHANGE);	//LSCP library will take care of the response	
	}
}

/**
 * @brief callback to generate data field for an outgoing LSCP frequency setting message
 * 
 *  The LSCP library will invoke this callback when the application needs to generate a setting response message to the android board, 
 *  per its request, letting it know what the frequency was set to in response to the frequency setting message the android board just sent down.
 *  
 *  It's not anticipated that the application will need to send an unsolicited frequency setting message to the android board, as frequency
 *  shouldn't be changing unless instructed by the android board.
 *
 * @param 
 * 
 * @return cJSON* a dynamically allocated cJSON struct containing the LSCP frequency setting message data field
 */
cJSON* generate_data_field_for_frequency_setting_msg_cb(void)
{
	cJSON *LSCP_data_field;
	float output_frequency;
	
	output_frequency = get_working_frequency_setting();
	
	LSCP_data_field = cJSON_CreateNumber(output_frequency);
	
	return(LSCP_data_field);
}
#pragma endregion "callback implementations related to the frequency setting"

#pragma region "callback implementations related to the shape setting"
/**
 * @brief callback to handle incoming setting message for local shape setting
 * 
 *  The LSCP library will invoke this callback when the android board has sent down a shape
 *  setting that we need to apply. Integer enum that for now represents DC or sine wave output.
 *  
 * @param shape_data_object data field of LSCP shape setting message, encoded as a cJSON data struct
 * 
 * @return void
 */
void local_setting_msg_cb_shape(cJSON *shape_data_object)
{
    int32_t dirty_shape;
    
    dirty_shape = shape_data_object->valueint;
    
    if(simple_validate_setting(dirty_shape, SHAPE_DC, SHAPE_SINE))
    {
        set_shape_setting((output_shape_type)dirty_shape, DO_NOT_NOTIFY_ANDROID_OF_SETTING_CHANGE);
    }
    
}
/**
 * @brief callback to generate data field for an outgoing LSCP shape setting message
 * 
 * The LSCP library will invoke this callback for the following scenarios:
 * 1) The application needs to generate a setting response message to the android board, per its request, letting it know what the actual shape is
 *    in response to the shape setting message the android board just sent down.
 * 2) The application needs to initiate the generation of a setting message to tell the android what the present shape output is. 
 *    This scenario only occurs if the firmware made the change unprovoked and needs to update the Android.
 * 
 * @param none
 * 
 * @return cJSON* a dynamically allocated cJSON struct containing the LSCP shape setting message data field
 */
cJSON* generate_data_field_for_shape_setting_msg_cb(void)
{
	cJSON *LSCP_data_field;
	output_shape_type shape;
	
	shape = get_working_shape_setting();
	
	LSCP_data_field = cJSON_CreateNumber(shape);
		
	return(LSCP_data_field);	
}
#pragma endregion "callback implementations related to the shape setting"

#pragma region "callback implementations related to the voltage range setting"
/**
 * @brief callback to handle incoming setting message for local voltage range setting
 * 
 * The LSCP library will invoke this callback when the android board has sent down a voltage range
 * setting that we need to apply.
 * 
 * @param voltage_range_data_object data field of LSCP voltage range setting message, encoded as a cJSON data struct
 * 
 * @return void
 */
void local_setting_msg_cb_voltage_range(cJSON *voltage_range_data_object)
{
	int32_t dirty_voltage_range;
	
	//extract the integer value from the cJSON data struct
	//we know the data type is an integer number based on the simple sources LSCP protocol definition
	dirty_voltage_range = voltage_range_data_object->valueint;				
	
	if(simple_validate_setting(dirty_voltage_range, VRANGE_10mV, VRANGE_100V))
	{
		set_voltage_range_setting((voltage_range_type)dirty_voltage_range, DO_NOT_NOTIFY_ANDROID_OF_SETTING_CHANGE);	//LSCP library will take care of the response
	}
}

/**
 * @brief callback to generate data field for an outgoing LSCP voltage range setting message
 * 
 * The LSCP library will invoke this callback for the following scenarios:
 * 1) The application needs to generate a setting response message to the android board, per its request, letting it know what the actual voltage range is
 *    in response to the voltage range setting message the android board just sent down.
 * 2) The application needs to initiate the generation of a setting message to tell the android what the present voltage range is. This scenario 
 *     could occur if the simple source is in auto range mode and the android board needs to be notified that the range has changed.
 * 
 * @param none
 * 
 * @return cJSON* a dynamically allocated cJSON struct containing the LSCP voltage setting message data field
 */
cJSON* generate_data_field_for_voltage_range_setting_msg_cb(void)
{
	cJSON *LSCP_data_field;
	voltage_range_type voltage_range;
	
	voltage_range = get_working_voltage_range_setting();
	
	LSCP_data_field = cJSON_CreateNumber(voltage_range);
		
	return(LSCP_data_field);	
}
#pragma endregion "callback implementations related to the voltage range setting"

#pragma region "callback implementations related to the voltage autorange enabled setting"
/**
 * @brief callback to handle incoming setting message for local voltage autorange setting
 * 
 * The LSCP library will invoke this callback when the android board has sent down the voltage autorange
 * setting that we need to apply. It's a boolean stating weather voltage autoranging is enabled or disabled.
 * 
 * This is a place holder in the protocol for future growth. Firmware doesn't consume this setting in any way now.
 * 
 * @param voltage_autorange_data_object data field of LSCP voltage autorange setting message, encoded as a cJSON data struct
 * 
 * @return void
 */
void local_setting_msg_cb_voltage_autorange_enabled(cJSON *voltage_autorange_data_object)
{
	bool dirty_voltage_autorange_enabled;
	
	if((voltage_autorange_data_object->type >= cJSON_False) && (voltage_autorange_data_object->type < cJSON_NULL)) //check to make sure cJSON type value isn't corrupt
	{
    	if(voltage_autorange_data_object->type == cJSON_True)
    	    dirty_voltage_autorange_enabled = true;
    	else
    	    dirty_voltage_autorange_enabled = false;

    	set_voltage_autorange_setting(dirty_voltage_autorange_enabled, DO_NOT_NOTIFY_ANDROID_OF_SETTING_CHANGE);	//LSCP library will take care of the response
	}
}

/**
 * @brief callback to generate data field for an outgoing LSCP voltage autorange setting message
 * 
 * The LSCP library will invoke this callback for the following scenarios:
 * 1) The application needs to generate a setting response message to the android board, per its request, letting it know what the actual voltage autorange 
 *    setting is in response to the voltage autorange setting message the android board just sent down.
 * 2) The application needs to initiate the generation of a setting message to tell the android what if voltage autorange is enabled. 
 * 
 * @param none
 * 
 * @return cJSON* a dynamically allocated cJSON struct containing the LSCP voltage autorange setting message data field
 */
cJSON* generate_data_field_for_voltage_autorange_enabled_setting_msg_cb(void)
{
	cJSON *LSCP_data_field;
	bool voltage_autorange_enabled;
	
	voltage_autorange_enabled = is_voltage_autorange_enabled();
	
	LSCP_data_field = cJSON_CreateBool(voltage_autorange_enabled);
		
	return(LSCP_data_field);	
}
#pragma endregion "callback implementations related to the voltage autorange enabled setting"

#pragma region "callback implementations related to the voltage level setting"
/**
 * @brief callback to handle incoming setting message for local voltage level setting
 * 
 * The LSCP library will invoke this callback when the android board has sent down the voltage level
 * setting that we need to apply. It's a simple structure comprised of a desired output amplitude and offset as doubles
 * 
 * @param voltage_level_data_object data field of LSCP voltage level setting message, encoded as a cJSON data struct
 * 
 * @return void
 */
void local_setting_msg_cb_voltage_level(cJSON *voltage_level_data_object)
{
	output_level_type dirty_voltage_level;
	
    dirty_voltage_level.amplitude = (float)(cJSON_GetObjectItem(voltage_level_data_object, "Amplitude")->valuedouble);
    dirty_voltage_level.offset = (float)(cJSON_GetObjectItem(voltage_level_data_object, "Offset")->valuedouble);
    
    if(validate_voltage_level_setting(dirty_voltage_level))
    {
        set_voltage_level_setting(dirty_voltage_level, DO_NOT_NOTIFY_ANDROID_OF_SETTING_CHANGE);
    }    
}

/**
 * @brief callback to generate data field for an outgoing LSCP voltage level setting message
 * 
 * The LSCP library will invoke this callback for the following scenarios:
 * 1) The application needs to generate a setting response message to the android board, per its request, letting it know what the actual voltage level 
 *    setting is in response to the voltage level setting message the android board just sent down.
 * 2) The application needs to initiate the generation of a setting message to tell the android what the voltage level is. Unlikely use case. 
 * 
 * @param none
 * 
 * @return cJSON* a dynamically allocated cJSON struct containing the LSCP voltage level setting message data field
 */
cJSON* generate_data_field_for_voltage_level_setting_msg_cb(void)
{
	cJSON *LSCP_data_field;
	output_level_type voltage_level;
	
	voltage_level = get_working_voltage_level_setting();
	
	LSCP_data_field = cJSON_CreateObject();
    
    cJSON_AddNumberToObject(LSCP_data_field, "Amplitude", voltage_level.amplitude);
    cJSON_AddNumberToObject(LSCP_data_field, "Offset", voltage_level.offset);
		
	return(LSCP_data_field);	
}
#pragma endregion "callback implementations related to the voltage level setting"

#pragma region "callback implementations related to the current level setting"
/**
 * @brief callback to handle incoming setting message for local current level setting
 * 
 * The LSCP library will invoke this callback when the android board has sent down the current level
 * setting that we need to apply. It's a simple structure comprised of a desired output amplitude and offset as doubles
 * 
 * @param current_level_data_object data field of LSCP current level setting message, encoded as a cJSON data struct
 * 
 * @return void
 */
void local_setting_msg_cb_current_level(cJSON *current_level_data_object)
{
	output_level_type dirty_current_level;
	
    dirty_current_level.amplitude = (float)(cJSON_GetObjectItem(current_level_data_object, "Amplitude")->valuedouble);
    dirty_current_level.offset = (float)(cJSON_GetObjectItem(current_level_data_object, "Offset")->valuedouble);
    
    if(validate_current_level_setting(dirty_current_level))
    {
        set_current_level_setting(dirty_current_level, DO_NOT_NOTIFY_ANDROID_OF_SETTING_CHANGE);
    }    
}

/**
 * @brief callback to generate data field for an outgoing LSCP current level setting message
 * 
 * The LSCP library will invoke this callback for the following scenarios:
 * 1) The application needs to generate a setting response message to the android board, per its request, letting it know what the actual current level 
 *    setting is in response to the current level setting message the android board just sent down.
 * 2) The application needs to initiate the generation of a setting message to tell the android what the current level is. Unlikely use case. 
 * 
 * @param none
 * 
 * @return cJSON* a dynamically allocated cJSON struct containing the LSCP current level setting message data field
 */
cJSON* generate_data_field_for_current_level_setting_msg_cb(void)
{
	cJSON *LSCP_data_field;
	output_level_type current_level;
	
	current_level = get_working_current_level_setting();
	
	LSCP_data_field = cJSON_CreateObject();
    
    cJSON_AddNumberToObject(LSCP_data_field, "Amplitude", current_level.amplitude);
    cJSON_AddNumberToObject(LSCP_data_field, "Offset", current_level.offset);
		
	return(LSCP_data_field);	
}
#pragma endregion "callback implementations related to the current level setting"

#pragma region "callback implementations related to the current range setting"
/**
 * @brief callback to handle incoming setting message for local current range setting
 * 
 * The LSCP library will invoke this callback when the android board has sent down a current range
 * setting that we need to apply.
 
 * @param current_range_data_object data field of LSCP current range setting message, encoded as a cJSON data struct
 * 
 * @return void
 */
void local_setting_msg_cb_current_range(cJSON *current_range_data_object)
{
	uint32_t dirty_current_range;
	
	//extract the integer value from the cJSON data struct
	//we know the data type is an integer number based on the simple sources LSCP protocol definition
	dirty_current_range = current_range_data_object->valueint;
	
	if(validate_current_range_setting(dirty_current_range))
	{
		set_current_range_setting((current_range_type)dirty_current_range, DO_NOT_NOTIFY_ANDROID_OF_SETTING_CHANGE);	//LSCP library will take care of the response
	}
}

/**
 * @brief callback to generate data field for an outgoing LSCP current range setting message
 * 
 * The LSCP library will invoke this callback for the following scenarios:
 * 1) The application needs to generate a setting response message to the android board, per its request, letting it know what the actual current range is
 *    in response to the voltage range setting message the android board just sent down.
 * 2) The application needs to initiate the generation of a setting message to tell the android what the present current range is. This scenario
 *     could occur if the simple source is in auto range mode and the android board needs to be notified that the range has changed.
 
 * @param none
 * 
 * @return cJSON* a dynamically allocated cJSON struct containing the LSCP current setting message data field
 */
cJSON* generate_data_field_for_current_range_setting_msg_cb(void)
{
	cJSON *LSCP_data_field;
	current_range_type current_range;
	
	current_range = get_working_current_range_setting();
	
	LSCP_data_field = cJSON_CreateNumber(current_range);
	
	return(LSCP_data_field);
}
#pragma endregion "callback implementations related to the current range setting"

#pragma region "callback implementations related to the voltage autorange enabled setting"
/**
 * @brief callback to handle incoming setting message for local current autorange enabled setting
 * 
 * The LSCP library will invoke this callback when the android board has sent down the current autorange enabled
 * setting that we need to apply. It's a boolean stating weather current autoranging is enabled or disabled.
 * 
 * This is a place holder in the protocol for future growth. Firmware doesn't consume this setting in any way now.
 * 
 * @param current_autorange_data_object data field of LSCP current autorange setting message, encoded as a cJSON data struct
 * 
 * @return void
 */
void local_setting_msg_cb_current_autorange_enabled(cJSON *current_autorange_data_object)
{
	bool dirty_current_autorange_enabled;
	
	if((current_autorange_data_object->type >= cJSON_False) && (current_autorange_data_object->type < cJSON_NULL)) //check to make sure cJSON type value isn't corrupt
	{
    	if(current_autorange_data_object->type == cJSON_True)
    	    dirty_current_autorange_enabled = true;
    	else
    	    dirty_current_autorange_enabled = false;

    	set_current_autorange_enabled_setting(dirty_current_autorange_enabled, DO_NOT_NOTIFY_ANDROID_OF_SETTING_CHANGE);	//LSCP library will take care of the response
	}
}

/**
 * @brief callback to generate data field for an outgoing LSCP current autorange enabled setting message
 * 
 * The LSCP library will invoke this callback for the following scenarios:
 * 1) The application needs to generate a setting response message to the android board, per its request, letting it know what the actual current autorange enabled
 *    setting is in response to the current autorange enabled setting message the android board just sent down.
 * 2) The application needs to initiate the generation of a setting message to tell the android what if current autorange is enabled. 
 * 
 * @param none
 * 
 * @return cJSON* a dynamically allocated cJSON struct containing the LSCP current autorange setting message data field
 */
cJSON* generate_data_field_for_current_autorange_enabled_setting_msg_cb(void)
{
	cJSON *LSCP_data_field;
	bool current_autorange_enabled;
	
	current_autorange_enabled = is_current_autorange_enabled();
	
	LSCP_data_field = cJSON_CreateBool(current_autorange_enabled);
		
	return(LSCP_data_field);	
}
#pragma endregion "callback implementations related to the voltage autorange enabled setting"

#pragma region "callback implementations related to the current compliance range setting"
/**
 * @brief callback to handle incoming setting message for local current compliance range setting
 * 
 * The LSCP library will invoke this callback when the android board has sent down a current compliance range
 * setting that we need to apply.
 * 
 * @param current_compliance_range_data_object data field of LSCP current compliance range setting message, encoded as a cJSON data struct
 * 
 * @return void
 */
void local_setting_msg_cb_current_compliance_range(cJSON *current_compliance_range_data_object)
{
	int32_t dirty_current_compliance_range;
	
	//extract the integer value from the cJSON data struct
	//we know the data type is an integer number based on the simple sources LSCP protocol definition
	dirty_current_compliance_range = current_compliance_range_data_object->valueint;				
	
	if(simple_validate_setting(dirty_current_compliance_range, I_COMPLIANCE_10V, I_COMPLIANCE_100V))
	{
		set_current_compliance_range_setting((current_compliance_range_type) dirty_current_compliance_range, DO_NOT_NOTIFY_ANDROID_OF_SETTING_CHANGE);	//LSCP library will take care of the response
	}
}

/**
 * @brief callback to generate data field for an outgoing LSCP current compliance range setting message
 * 
 * The LSCP library will invoke this callback for the following scenarios:
 * 1) The application needs to generate a setting response message to the android board, per its request, letting it know what the actual current compliance range is
 *    in response to the current compliance range setting message the android board just sent down.
 * 2) The application needs to initiate the generation of a setting message to tell the android what the present current compliance range is. This scenario 
 *     could occur if the simple source is in auto range mode and the android board needs to be notified that the range has changed.
 * 
 * @param none
 * 
 * @return cJSON* a dynamically allocated cJSON struct containing the LSCP current compliance setting message data field
 */
cJSON* generate_data_field_for_current_compliance_range_setting_msg_cb(void)
{
	cJSON *LSCP_data_field;
	current_compliance_range_type current_compliance_range;
	
	current_compliance_range = get_working_current_compliance_range_setting();
	
	LSCP_data_field = cJSON_CreateNumber(current_compliance_range);
		
	return(LSCP_data_field);	
}
#pragma endregion "callback implementations related to the current compliance range setting"

#pragma region "callback implementations related to the current compliance status setting"

/**
 * @brief callback to generate data field for an outgoing LSCP current compliance status setting message
 * 
 * The LSCP library will invoke this callback for the following scenario:
 * 1) The application needs to initiate the generation of a setting message to tell the android that the output is in current compliance.
 * 
 * @param none
 * 
 * @return cJSON* a dynamically allocated cJSON struct containing the LSCP current compliance status setting message data field
 */
cJSON* generate_data_field_for_current_compliance_status_setting_msg_cb(void)
{
	cJSON *LSCP_data_field;
	bool current_output_is_in_compliance;
	
	current_output_is_in_compliance = get_current_output_compliance_status_setting();
	
	LSCP_data_field = cJSON_CreateBool(current_output_is_in_compliance);
		
	return(LSCP_data_field);	
}
#pragma endregion "callback implementations related to the current compliance status setting"

#pragma region "callback implementations related to the voltage protection status setting"

/**
 * @brief callback to generate data field for an outgoing LSCP voltage protection status setting message
 * 
 * The LSCP library will invoke this callback for the following scenario:
 * 1) The application needs to initiate the generation of a setting message to tell the android that the voltage output is in a protection state (current limit).
 * 
 * @param none
 * 
 * @return cJSON* a dynamically allocated cJSON struct containing the LSCP voltage protection status setting message data field
 */
cJSON* generate_data_field_for_voltage_protection_setting_msg_cb(void)
{
	cJSON *LSCP_data_field;
	bool voltage_output_is_in_current_limit;
	
	voltage_output_is_in_current_limit = get_voltage_output_protection_status_setting();
	
	LSCP_data_field = cJSON_CreateBool(voltage_output_is_in_current_limit);
		
	return(LSCP_data_field);	
}
#pragma endregion "callback implementations related to the voltage protection status setting"

#pragma region "callback implementations related to the terminals setting"
/**
 * @brief callback to handle incoming setting message for local terminals setting
 * 
 * The LSCP library will invoke this callback when the android board has sent down a terminals selection (front/rear)
 * setting that we need to apply.
 * 
 * @param terminals_data_object data field of LSCP terminals setting message, encoded as a cJSON data struct
 * 
 * @return void
 */
void local_setting_msg_cb_terminals(cJSON *terminals_data_object)
{
	int32_t dirty_terminals_setting;
	
	//extract the integer value from the cJSON data struct
	//we know the data type is an integer number based on the simple sources LSCP protocol definition
	dirty_terminals_setting = terminals_data_object->valueint;				
	
	if(simple_validate_setting(dirty_terminals_setting, TERMINALS_FRONT, TERMINALS_REAR))
	{
		set_terminals_setting((terminal_selection_type) dirty_terminals_setting, DO_NOT_NOTIFY_ANDROID_OF_SETTING_CHANGE);	//LSCP library will take care of the response
	}
}

/**
 * @brief callback to generate data field for an outgoing LSCP terminals setting message
 * 
 * The LSCP library will invoke this callback for the following scenarios:
 * 1) The application needs to generate a setting response message to the android board, per its request, letting it know what the actual terminals setting is
 *    in response to the terminals setting message the android board just sent down.
 * 2) The application needs to initiate the generation of a setting message to tell the android what the terminals setting is.
 *    No use cases are defined yet where this firmware would need to automatically update the terminals selection
 * 
 * @param none
 * 
 * @return cJSON* a dynamically allocated cJSON struct containing the LSCP current compliance setting message data field
 */
cJSON* generate_data_field_for_terminals_setting_msg_cb(void)
{
	cJSON *LSCP_data_field;
	terminal_selection_type terminals_setting;
	
	terminals_setting = get_working_terminals_setting();
	
	LSCP_data_field = cJSON_CreateNumber(terminals_setting);
		
	return(LSCP_data_field);	
}
#pragma endregion "callback implementations related to the terminals setting"

#pragma region "callback implementations related to the info setting"

/**
 * @brief callback to generate data field for an outgoing LSCP info setting message
 * 
 * The LSCP library will invoke this callback for the following scenario:
 * 1)  Per the simple sources LSCP protocol implementation, the "Info" setting message is sent as the result of a "Sync" command being
 *    sent from the Android board to this application. 
 *    
 *    The packet sequences is as follows:
 *    1) Android sends "Sync" command to sources Atmel micro.
 *    2) The "Sync" local command callback issues "info" setting to Android board.
 *    3) After info setting message is complete, the atmel micro will issue the "sync" command response packet, assuming the Android board populated the ID field.
 * 
 * @param none
 * 
 * @return cJSON* a dynamically allocated cJSON struct containing the LSCP current compliance status setting message data field
 */
cJSON* generate_data_field_for_info_setting_msg_cb(void)
{
	cJSON *LSCP_data_field;
    instrument_info_type instrument_info;
    
    LSCP_data_field = cJSON_CreateObject();
    
    instrument_info = get_instrument_info();
    
    cJSON_AddStringToObject(LSCP_data_field, "ModelNumber", instrument_info.model_number);
	cJSON_AddStringToObject(LSCP_data_field, "FirmwareType", instrument_info.firmware_type);
    cJSON_AddStringToObject(LSCP_data_field, "VersionName", instrument_info.version_name);
    cJSON_AddStringToObject(LSCP_data_field, "BoardRevision", instrument_info.board_revision);
    cJSON_AddBoolToObject(LSCP_data_field, "CurrentBoardPresent", instrument_info.current_capability);    
		
	return(LSCP_data_field);	
}
#pragma endregion "callback implementations related to the info setting"

#pragma region "callback implementations related to the CalData setting"
/**
 * @brief callback to handle incoming setting message for CalData setting
 * 
 * The LSCP library will invoke this callback when the android board has sent down a Calibration Data setting message
 * 
 * Message Includes:
 * Serial Number                
 * Ac Function Enabled Flag
 * Calibration Date             
 * Calibration Next Due         
 * Current Gains and Offsets
 * Voltage Gains and Offsets
 * 
 * @param caldata_data_object data field of LSCP CalData setting message, encoded as a cJSON data struct
 * 
 * @return void
 */
void local_setting_msg_cb_caldata(cJSON *caldata_data_object)
{
    uint32_t counter = 0;
	calibration_data_type dirty_calibration_data;
    
    cJSON *current_cal_object;
    cJSON *current_offsets;
    cJSON *current_gains;
    cJSON *voltage_cal_object;
    cJSON *voltage_offsets;
    cJSON *voltage_gains;
    
    //get serial number
    //strcpy(dirty_calibration_data.serial_number, cJSON_GetObjectItem(caldata_data_object, "SerialNumber")->valuestring);
    
    //get AC enabled field
    dirty_calibration_data.ac_enabled = (bool)(cJSON_GetObjectItem(caldata_data_object, "AcFunctionalityEnabled")->type); //booleans value types aren't numbers, they are their own cJSON type. 0 = cJSON_False, 1 = cJSON_true
    
    //get calibration date
    //strcpy(dirty_calibration_data.date, cJSON_GetObjectItem(caldata_data_object, "Date")->valuestring);
    
    //get next calibration date
    //strcpy(dirty_calibration_data.due_date, cJSON_GetObjectItem(caldata_data_object, "DueDate")->valuestring);

    //get current
    current_cal_object = cJSON_GetObjectItem(caldata_data_object, "Current");
    //offsets and gains are arrays
    current_offsets = cJSON_GetObjectItem(current_cal_object, "Offsets");
    current_gains = cJSON_GetObjectItem(current_cal_object, "Gains");
    
   for(counter = 0; counter < NUMBER_OF_CURRENT_CAL_POINTS; counter++)
    {
        dirty_calibration_data.current.offsets[counter] = (float)(cJSON_GetArrayItem(current_offsets, counter)->valuedouble);
        dirty_calibration_data.current.gains[counter] = (float)(cJSON_GetArrayItem(current_gains, counter)->valuedouble);
    }    
    
    //get voltage
    voltage_cal_object = cJSON_GetObjectItem(caldata_data_object, "Voltage");
    //offsets and gains are arrays
    voltage_offsets = cJSON_GetObjectItem(voltage_cal_object, "Offsets");
    voltage_gains = cJSON_GetObjectItem(voltage_cal_object, "Gains");
    
    for(counter = 0; counter < NUMBER_OF_CURRENT_CAL_POINTS; counter++)
    {
        dirty_calibration_data.voltage.offsets[counter] = (float)(cJSON_GetArrayItem(voltage_offsets, counter)->valuedouble);
        dirty_calibration_data.voltage.gains[counter] = (float)(cJSON_GetArrayItem(voltage_gains, counter)->valuedouble);
    }
    
    set_calibraton_data(dirty_calibration_data, DO_NOT_NOTIFY_ANDROID_OF_SETTING_CHANGE);
    
	//if(simple_validate_setting(dirty_terminals_setting, TERMINALS_FRONT, TERMINALS_REAR))
	//{
	//	set_terminals_setting((terminal_selection_type) dirty_terminals_setting, DO_NOT_NOTIFY_ANDROID_OF_SETTING_CHANGE);	//LSCP library will take care of the response
	//}
}

/**
 * @brief callback to generate data field for an outgoing LSCP terminals setting message
 * 
 * The LSCP library will invoke this callback for the following scenarios:
 * 1) The application needs to generate a setting response message to the android board, per its request, letting it know the current state of the calibration data
 *    in RAM in response to the CalData setting message the android board just sent down.
 * 2) The application needs to initiate the generation of a setting message to tell the android what the terminals setting is.
 *    This scenario occurs as part of the Sync command message sequence.
 * 
 * @param none
 * 
 * @return cJSON* a dynamically allocated cJSON struct containing the LSCP current compliance setting message data field
 */
cJSON* generate_data_field_for_caldata_setting_msg_cb(void)
{
	cJSON *LSCP_data_field;
    cJSON *current_cal_object, *current_offsets, *current_gains;
    cJSON *voltage_cal_object, *voltage_offsets, *voltage_gains;    
    //calibration_data_type calibration_data;
    
    //calibration_data = get_calibration_data();
    
    voltage_offsets = cJSON_CreateFloatArray((const float*)(&calibration_data.voltage.offsets),NUMBER_OF_VOLTAGE_CAL_POINTS);
    voltage_gains = cJSON_CreateFloatArray((const float*)(&calibration_data.voltage.gains),NUMBER_OF_VOLTAGE_CAL_POINTS);
    voltage_cal_object = cJSON_CreateObject();
    cJSON_AddItemToObject(voltage_cal_object, "Offsets", voltage_offsets);
    cJSON_AddItemToObject(voltage_cal_object, "Gains", voltage_gains);
    
    current_offsets = cJSON_CreateFloatArray((const float*)(&calibration_data.current.offsets), NUMBER_OF_CURRENT_CAL_POINTS);
    current_gains = cJSON_CreateFloatArray((const float*)(&calibration_data.current.gains), NUMBER_OF_CURRENT_CAL_POINTS);
    current_cal_object = cJSON_CreateObject();
    cJSON_AddItemToObject(current_cal_object, "Offsets", current_offsets);
    cJSON_AddItemToObject(current_cal_object, "Gains", current_gains);
    
    LSCP_data_field = cJSON_CreateObject();
    cJSON_AddStringToObject(LSCP_data_field, "SerialNumber", calibration_data.serial_number);
    cJSON_AddBoolToObject(LSCP_data_field, "AcFunctionalityEnabled", calibration_data.ac_enabled);
    cJSON_AddStringToObject(LSCP_data_field, "Date", calibration_data.date);
    cJSON_AddStringToObject(LSCP_data_field, "DueDate", calibration_data.due_date);
    cJSON_AddItemToObject(LSCP_data_field, "Current", current_cal_object);
    cJSON_AddItemToObject(LSCP_data_field, "Voltage", voltage_cal_object);
    
    //voltage_offsets = cJSON_CreateFloatArray((const float*)(&calibration_data.voltage.offsets),NUMBER_OF_VOLTAGE_CAL_POINTS);
    //voltage_gains = cJSON_CreateFloatArray((const float*)(&calibration_data.voltage.gains),NUMBER_OF_VOLTAGE_CAL_POINTS);
    //voltage_cal_object = cJSON_CreateObject();
    //cJSON_AddItemToObject(voltage_cal_object, "Offsets", voltage_offsets);
    //cJSON_AddItemToObject(voltage_cal_object, "Gains", voltage_gains);
    //
    //current_offsets = cJSON_CreateFloatArray((const float*)(&calibration_data.current.offsets), NUMBER_OF_CURRENT_CAL_POINTS);
    //current_gains = cJSON_CreateFloatArray((const float*)(&calibration_data.current.gains), NUMBER_OF_CURRENT_CAL_POINTS);
    //current_cal_object = cJSON_CreateObject();
    //cJSON_AddItemToObject(current_cal_object, "Offsets", current_offsets);
    //cJSON_AddItemToObject(current_cal_object, "Gains", current_gains);
    //
    //LSCP_data_field = cJSON_CreateObject();    
    //cJSON_AddStringToObject(LSCP_data_field, "SerialNumber", calibration_data.serial_number);
    //cJSON_AddBoolToObject(LSCP_data_field, "AcFunctionalityEnabled", calibration_data.ac_enabled);
    //cJSON_AddStringToObject(LSCP_data_field, "Date", calibration_data.date);
    //cJSON_AddStringToObject(LSCP_data_field, "DueDate", calibration_data.due_date);
    //cJSON_AddItemToObject(LSCP_data_field, "Current", current_cal_object);
    //cJSON_AddItemToObject(LSCP_data_field, "Voltage", voltage_cal_object);
		
	return(LSCP_data_field);	
}
#pragma endregion "callback implementations related to the CalData setting"

#pragma region "callback implementations related to the reading update setting message"
cJSON* generate_data_field_for_input_reading_msg_cb(void)
{
    cJSON *json_data_obj, *json_inputA_obj, *json_inputB_obj, *json_inputC_obj, *json_inputD_obj;
    
    //create the lowest level objects - in this case, each input reading object
    json_inputA_obj = cJSON_CreateObject();
    cJSON_AddNumberToObject(json_inputA_obj, "rdg", 1.32456);
    cJSON_AddNumberToObject(json_inputA_obj, "st", 0);

    json_inputB_obj = cJSON_CreateObject();
    cJSON_AddNumberToObject(json_inputB_obj, "rdg", 100.563);
    cJSON_AddNumberToObject(json_inputB_obj, "st", 1);

    json_inputC_obj = cJSON_CreateObject();
    cJSON_AddNumberToObject(json_inputC_obj, "rdg", 0.00123);
    cJSON_AddNumberToObject(json_inputC_obj, "st", 5);

    json_inputD_obj = cJSON_CreateObject();
    cJSON_AddNumberToObject(json_inputD_obj, "rdg", 3.14159);
    cJSON_AddNumberToObject(json_inputD_obj, "st", 3);

    
    //next level up, each input reading object comprises a data object
    json_data_obj = cJSON_CreateObject();
    cJSON_AddItemToObject(json_data_obj, "InputA", json_inputA_obj);
    cJSON_AddItemToObject(json_data_obj, "InputB", json_inputB_obj);
    cJSON_AddItemToObject(json_data_obj, "InputC", json_inputC_obj);
    cJSON_AddItemToObject(json_data_obj, "InputD", json_inputD_obj);    
    
    return(json_data_obj);    
}
#pragma endregion "callback implementations related to the reading update setting message"