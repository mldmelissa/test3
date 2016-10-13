/** @file settings_manager.cpp
 *  @brief Implementation file for the simple sources settings getters, setters, and validation functions
 *    
 *  This module is responsible for storing the working values of an instrument settings, implementing validation functions for
 *  incoming setting messages, and making the calls to low level functions to set the hardware appropriately per the 
 *  setting requirements.
 *  
 *  Functions defined here in settings manager can be called from either:
 *  1) LSCP local setting message callbacks, needing to apply a setting coming in over the remote interface
 *  2) The application code that needs to update its own internal setting
 *  
 *  Regarding scenario two, per the LSCP protocol definition, when a local setting is updated, the embedded ARM may
 *  need to send an unsolicited setting message to the Android board, notifying it that a setting has changed. An
 *  example of this is if the firmware is monitoring the current output compliance status. If the output goes into
 *  compliance, the application code will call the set_current_compliance_status_setting(), passing in the paramter
 *  to notify the android that the setting has changed.
 *   
 *  
 *  @author Adam Porsch
 *  @bug No known bugs.
 */

#include "hal.h"
#include "settings_manager.h"
#include "output_control.h"
#include "sources_settings_callbacks.h"
#include "android_comm_interface_manager.h"

settings_type settings;
settings_type *settings_ptr;                    //TODO: REMOVE. HERE TO GET MEM ADDR OF STUCT TO SHOW UP IN IDE WINDOW

bool start_command_received = false;			//Used for power up. Don't apply any hardware settings until .NET code gives us the green light by sending the "start" command.


#pragma region "setting initialization functions"
//TODO: Add power-up init routines for settings struct
void load_settings_struct_with_default_values(void)
{
    settings.output_mode = VOLTAGE_MODE;                    //No 1:1 translation from output_mode to HW.
    settings.output_state_enabled = false;                  //LV and HV OUTSTAGESEL = 1 (neither one is fed to terminals)
    settings.calibration_locked = true;                     //WP pin is high
    settings.output_frequency = 0;
    settings.output_shape = SHAPE_DC;
    settings.voltage_range = VRANGE_10mV;                   //b/c U24 inputs are 0b11
    settings.voltage_autorange_enabled = false;             //TODO: verify this is what .NET defaults to.
    settings.output_voltage_level.amplitude = 0;   
    settings.output_voltage_level.offset = 0;       
    settings.output_current_level.amplitude = 0;         
    settings.output_current_level.offset = 0;
    settings.current_range = IRANGE_1uA;                    //Technically, I range is n/a since none of the relays are engaged
    settings.current_autorange_enabled = false;
    settings.current_compliance_range = I_COMPLIANCE_10V;   //No 1:1, both LV U29 and HV U14 are "disabled" (DAC in grounded)
    settings.current_compliace_active = false;
    settings.voltage_protection_active = false;
    settings.terminal_selection = TERMINALS_REAR;           //based off U45 PIO inputs being high and K20:22 relay config
    settings.instrument_info.model_number = "Model 155";
	settings.instrument_info.firmware_type = "155acdc";
    settings.instrument_info.version_name = "9/12/2016";
    settings.instrument_info.board_revision = "0";
    settings.instrument_info.current_capability = true;
    
    settings_ptr = &settings;								//TODO: REMOVE. HERE TO GET MEM ADDR OF STUCT TO SHOW UP IN IDE WINDOW
    
}

void test_init_function(void)
{
    //load_settings_struct_with_default_values();
    load_calibration_default_values();
    
    //settings.output_mode = CURRENT_MODE;
    //settings.current_compliance_range = I_COMPLIANCE_100V;
    //settings.output_state_enabled = true;

    //settings.output_mode = VOLTAGE_MODE;                    //No 1:1 translation from output_mode to HW.
    //settings.output_state_enabled = true;                  //LV and HV OUTSTAGESEL = 1 (neither one is fed to terminals)
    //settings.output_shape = SHAPE_SINE;
    //settings.voltage_range = VRANGE_10V;                   //b/c U24 inputs are 0b11
    ////settings.current_range = IRANGE_10mA;
    ////settings.current_compliance_range = I_COMPLIANCE_10V;
    //settings.output_voltage_level.amplitude = 5;
    //settings.output_voltage_level.offset = 0;
    //settings.output_frequency = 1e3;
    //settings.terminal_selection = TERMINALS_REAR;           //based off U45 PIO inputs being high and K20:22 relay config
    //
    //execute_enable_output_sequence();
    
    
    //generate_local_caldata_setting_message();    
}

#pragma endregion "setting initialization functions"

#pragma region "general setting manager functions"

void execute_start_command(void)
{
	start_command_received = true;

	if(settings.output_state_enabled == true)
	{
		execute_enable_output_sequence();
	}
}

bool simple_validate_setting(int32_t value_to_validate, int32_t low_test_value, int32_t high_test_value)
{
    bool valid_setting = false;		//init to fail until proven otherwise
    
    if((value_to_validate >= low_test_value ) && (value_to_validate <= high_test_value))
    {
        valid_setting = true;
    }
    
    return(valid_setting);
}

void execute_disable_output_sequence(void)
{
    bring_DAC_output_to_zero(settings.output_shape);
    configure_DAC_input_to_desired_output_stage(VOLTAGE_MODE, OUTSTG_SEL_BOTH);
    shunt_output_stage(OUTSTG_SEL_BOTH);
    disable_output_terminals();    
}

//TODO: Refactor the mess that is this function...
void execute_enable_output_sequence(void)
{
    void (*current_range_function_ptr)(current_range_type);
    output_stage_selection_type output_stage_selection = OUTSTG_SEL_LOW_VOLTAGE;
    
    current_range_function_ptr = &set_low_voltage_current_range_HW;         //default to low voltage stage to be safe    
    
    if((settings.output_mode == VOLTAGE_MODE && settings.voltage_range == VRANGE_100V)  || 
       (settings.output_mode == CURRENT_MODE && settings.current_compliance_range == I_COMPLIANCE_100V))
    {
        output_stage_selection = OUTSTG_SEL_HIGH_VOLTAGE;
        current_range_function_ptr = set_high_voltage_current_range_HW;        
    }
    
    if((settings.voltage_range) < VRANGE_100V && (settings.output_mode == VOLTAGE_MODE))
        set_low_voltage_voltage_range_HW(settings.voltage_range);     
    else
        set_low_voltage_voltage_range_HW(VRANGE_10mV);           //else, we're in high voltage stage or Current Mode, set LV range to known state
    
    //If I'm in voltage mode, i'm setting the I limit range.                                            Else, I'm setting the I out range.
        //current_range_function_ptr(current_range_type SETTABLE_CURRENT_LIMIT_RANGE_SETTING)   OR   current_range_function_ptr(settings.current_range)  
    if(settings.output_mode == CURRENT_MODE)                //TODO: REFACTOR THIS BY USING NEW SETTING PARAM TO SELECT I SHUNT RESISTOR FOR DESIRED I LIMIT SETTING
    {
        //current_range_function_ptr(settings.current_range);
        
        if(output_stage_selection == OUTSTG_SEL_HIGH_VOLTAGE)
        {
            set_high_voltage_current_range_HW(settings.current_range);          //active output stage
            set_low_voltage_current_range_HW(IRANGE_100mA);                     //non-active stage I range relays set to known state            
        }
        else
        {
            set_low_voltage_current_range_HW(settings.current_range);
            set_high_voltage_current_range_HW(IRANGE_HV_AC_BYPASS);            
        }
    }
    else
    {                                                      
        if(settings.voltage_range == VRANGE_100V)
        {
           current_range_function_ptr(IRANGE_HV_AC_BYPASS) ;
           set_low_voltage_current_range_HW(IRANGE_100mA);          //set I range of UNUSED output stage to known state
        }           
        else
        {
            current_range_function_ptr(IRANGE_100mA);
            set_high_voltage_current_range_HW(IRANGE_HV_AC_BYPASS);
        } 
    }
       
    //set_DAC0_value_to_programmable_current_or_compliance_limit_value().... //TODO:
    configure_DAC_input_to_desired_output_stage(settings.output_mode, output_stage_selection);
    select_front_or_rear_output_terminals(settings.terminal_selection);
    connect_output_stage_to_output_terminals(output_stage_selection);
    unshunt_output_stage(output_stage_selection);
    execute_output_shape_change_sequence();             //will apply frequency, output levels, DC vs Sine shape, 
    
}

float get_full_scale_voltage_range_value(voltage_range_type voltage_range)
{
    float fs_range_value;
    
    switch(voltage_range)
    {
        case VRANGE_100V:
            fs_range_value = FULL_SCALE_100V_RANGE_VALUE;
            break;
            
        case VRANGE_10V:
            fs_range_value = FULL_SCALE_10V_RANGE_VALUE;
            break;
        
        case VRANGE_1V:
            fs_range_value = FULL_SCALE_1V_RANGE_VALUE;
            break;
        
        case VRANGE_100mV:
            fs_range_value = FULL_SCALE_100MV_RANGE_VALUE;
            break;
        
        case VRANGE_10mV:
        default:
            fs_range_value =   FULL_SCALE_10MV_RANGE_VALUE;      
            break;
    }
    
    return(fs_range_value);    
}

float get_full_scale_current_range_value(current_range_type current_range)
{
    float fs_range_value;
    
    switch(current_range)
    {
        case IRANGE_100mA:
            fs_range_value = FULL_SCALE_100MA_RANGE_VALUE;
            break;
        
        case IRANGE_10mA:
            fs_range_value = FULL_SCALE_10MA_RANGE_VALUE;
            break;
        
        case IRANGE_1mA:
            fs_range_value = FULL_SCALE_1MA_RANGE_VALUE;
            break;
        
        case IRANGE_100uA:
            fs_range_value = FULL_SCALE_100UA_RANGE_VALUE;
            break;
        
        case IRANGE_10uA:
            fs_range_value = FULL_SCALE_10UA_RANGE_VALUE;
            break;
        
        case IRANGE_1uA:
        default:
            fs_range_value = FULL_SCALE_1UA_RANGE_VALUE;
            break;
    }
    
    return(fs_range_value);
}

#pragma endregion "general setting manager functionss"

#pragma region "mode setting support functions"

void set_mode_setting(output_mode_type validated_mode, setting_android_notify_type notify_android)
{    
    settings.output_mode = validated_mode;
    
    if(notify_android == NOTIFY_ANDROID_OF_SETTING_CHANGE)
    {
        generate_local_setting_message(SETTING_STRING_MODE);
    }
}

output_mode_type get_working_mode_setting(void)
{
    return(settings.output_mode);
}
#pragma endregion "mode setting support functions"

#pragma region "output state support functions"

void set_output_state_enabled_setting(bool enable_output, setting_android_notify_type notify_android)
{
	if(start_command_received)
	{
		if(enable_output)
		{
			execute_enable_output_sequence();
		}
		else
		{
			execute_disable_output_sequence();
		}
	}
	    
    //update the working value now that the hardware has been updated
    settings.output_state_enabled = enable_output;
    
    if(notify_android == NOTIFY_ANDROID_OF_SETTING_CHANGE)
    {
        generate_local_setting_message(SETTING_STRING_OUTPUT_STATE);
    }
}

bool is_output_state_enabled(void)
{
    return(settings.output_state_enabled);
}
#pragma endregion "output state support functions"

#pragma region "CalLocked support functions"

void set_calibration_locked_setting(bool calibration_locked, setting_android_notify_type notify_android)
{    
    if(calibration_locked)
        ENABLE_EEPROM_WP;
    else
        DISABLE_EEPROM_WP;
   
    settings.calibration_locked = calibration_locked;
    
    if(notify_android == NOTIFY_ANDROID_OF_SETTING_CHANGE)
    {
        generate_local_setting_message(SETTING_STRING_CALLOCKED);
    }
}

bool is_calibration_locked(void)
{
    return(settings.calibration_locked);
}
#pragma endregion "CalLocked support functions"

#pragma region "frequency setting support functions"
bool validate_frequency_setting(float pending_frequency)
{
    bool valid_setting = false;		//init to fail until proved otherwise
    
    if((pending_frequency >= MINIMUM_FREQUENCY_IN_HZ ) && (pending_frequency <= MAXIMUM_FREQUENCY_IN_HZ))
    {
        valid_setting = true;
    }
    
    return(valid_setting);
}

void set_frequency_setting(float validated_frequency, setting_android_notify_type notify_android)
{
    //if(settings.output_state_enabled && (settings.output_shape == SHAPE_SINE))
    //The phase increment can be updated regardless of shape setting
    set_output_frequency(validated_frequency);

    settings.output_frequency = validated_frequency;
    
    if(notify_android == NOTIFY_ANDROID_OF_SETTING_CHANGE)
    {
        generate_local_setting_message(SETTING_STRING_FREQUENCY);
    }
}

float get_working_frequency_setting(void)
{
    return(settings.output_frequency);
}
#pragma endregion "frequency setting support functions"

#pragma region "shape setting support functions"

void execute_output_shape_change_sequence(void)
{
    float amplitude, offset, full_scale_divisor;
    
    if(settings.output_mode == VOLTAGE_MODE)
    {
        amplitude = settings.output_voltage_level.amplitude;
        offset = settings.output_voltage_level.offset;
        full_scale_divisor = get_full_scale_voltage_range_value(settings.voltage_range);
    }
    else
    {
        amplitude = settings.output_current_level.amplitude;
        offset = settings.output_current_level.offset;
        full_scale_divisor = get_full_scale_current_range_value(settings.current_range);
    }
    
    set_output_shape(settings.output_shape, amplitude, offset, full_scale_divisor);
}

void set_shape_setting(output_shape_type validated_shape, setting_android_notify_type notify_android)
{    
    settings.output_shape = validated_shape;
    
    if(settings.output_state_enabled == true)
    {
        execute_output_shape_change_sequence();
    }
    
    if(notify_android == NOTIFY_ANDROID_OF_SETTING_CHANGE)
    {
        generate_local_setting_message(SETTING_STRING_SHAPE);
    }
}

output_shape_type get_working_shape_setting(void)
{
    return(settings.output_shape);
}

#pragma endregion "shape setting support functions"

#pragma region "voltage range setting support functions"

void set_voltage_range_setting(voltage_range_type validated_voltage_range, setting_android_notify_type notify_android)
{
    output_stage_selection_type desired_output_stage_selection = OUTSTG_SEL_LOW_VOLTAGE;
    
    if(settings.output_state_enabled == true && settings.output_mode == VOLTAGE_MODE)
    {
        bring_DAC_output_to_zero(settings.output_shape);
        
        if(validated_voltage_range < VRANGE_100V)             
        {           
           set_low_voltage_voltage_range_HW(validated_voltage_range);    
        }
        else if(validated_voltage_range == VRANGE_100V)                                                  
        {
            desired_output_stage_selection = OUTSTG_SEL_HIGH_VOLTAGE;
        }
        
        shunt_output_stage(desired_output_stage_selection);
        configure_DAC_input_to_desired_output_stage(VOLTAGE_MODE, desired_output_stage_selection);
        unshunt_output_stage(desired_output_stage_selection);  
        execute_output_shape_change_sequence();											 //will apply frequency, output levels, DC vs Sine shape,          
        connect_output_stage_to_output_terminals(desired_output_stage_selection);        //call needed in case we're switching b/t HV and LV ranges      
    }                  
	
	settings.voltage_range = validated_voltage_range;	
	
	if(notify_android == NOTIFY_ANDROID_OF_SETTING_CHANGE)
	{
		generate_local_setting_message(SETTING_STRING_VOLTAGE_RANGE);
	}
}



voltage_range_type get_working_voltage_range_setting(void)
{
	return(settings.voltage_range);
}

#pragma endregion "voltage range setting support functions"

#pragma region "voltage autorange setting support functions"

void set_voltage_autorange_setting(bool voltage_autorange_enabled, setting_android_notify_type notify_android)
{
    //This setting is a place holder for future growth. Android takes care of all simple source auto ranging.
    
    settings.voltage_autorange_enabled = voltage_autorange_enabled;
    
    if(notify_android == NOTIFY_ANDROID_OF_SETTING_CHANGE)
    {
        generate_local_setting_message(SETTING_STRING_VOLTAGE_AUTORANGE_ENABLED);
    }
}



bool is_voltage_autorange_enabled(void)
{
    return(settings.voltage_autorange_enabled);
}

#pragma endregion "voltage autorange setting support functions"

#pragma region "voltage level setting support functions"

bool validate_voltage_level_setting(output_level_type pending_voltage_level)
{
    bool valid_setting = false;		//init to fail until proved otherwise
    float max_test_output_level = 0;
    float min_test_output_level = 0;
    float test_output_level = 0;
    
    test_output_level = pending_voltage_level.amplitude;
    
    if(settings.output_shape != SHAPE_DC)                   //ignore offset for DC voltage level settings. Only amplitude is used in DC mode.
        test_output_level += pending_voltage_level.offset;  //Not sure if android will send an offset value when in DC mode. This will protect against that.
    
    //is the desired combined amplitude and offset greater than the voltage range I'm presently set to?
    switch(settings.voltage_range)
    {
        case VRANGE_100V:
            min_test_output_level = MIN_VOLTAGE_OUTPUT_100V_RANGE;
            max_test_output_level = MAX_VOLTAGE_OUTPUT_100V_RANGE;
            break;
        
        case VRANGE_10V:
            min_test_output_level = MIN_VOLTAGE_OUTPUT_10V_RANGE;
            max_test_output_level = MAX_VOLTAGE_OUTPUT_10V_RANGE;
            break;
        
        case VRANGE_1V:
            min_test_output_level = MIN_VOLTAGE_OUTPUT_1V_RANGE;
            max_test_output_level = MAX_VOLTAGE_OUTPUT_1V_RANGE;
            break;
            
        case VRANGE_100mV:
            min_test_output_level = MIN_VOLTAGE_OUTPUT_100MV_RANGE;
            max_test_output_level = MAX_VOLTAGE_OUTPUT_100MV_RANGE;
            break;
            
        case VRANGE_10mV:
        default:
            min_test_output_level = MIN_VOLTAGE_OUTPUT_10MV_RANGE;
            max_test_output_level = MAX_VOLTAGE_OUTPUT_10MV_RANGE;
            break;
    }

    if((test_output_level >= min_test_output_level) && (test_output_level <= max_test_output_level))
    {
        valid_setting = true;
    }
    
    return(valid_setting);   
}

void set_voltage_level_setting(output_level_type validated_voltage_level, setting_android_notify_type notify_android)
{
    settings.output_voltage_level.amplitude = validated_voltage_level.amplitude;
    settings.output_voltage_level.offset = validated_voltage_level.offset;

    if(settings.output_state_enabled == true && settings.output_mode == VOLTAGE_MODE)
    {
        if(get_presently_selected_output_stage() == OUTSTG_SEL_LOW_VOLTAGE)
        {
            validated_voltage_level.amplitude = -validated_voltage_level.amplitude;
            validated_voltage_level.offset = -validated_voltage_level.offset;
        }
        
        if(settings.output_shape == SHAPE_DC)
        {       
            update_DAC_output_while_in_DC_mode(validated_voltage_level.amplitude, get_full_scale_voltage_range_value(settings.voltage_range));
        }
        else
        {
            generate_DAC_code_table(validated_voltage_level.amplitude, validated_voltage_level.offset, get_full_scale_voltage_range_value(settings.voltage_range));
        }
    }    
    
    if(notify_android == NOTIFY_ANDROID_OF_SETTING_CHANGE)
    {
        generate_local_setting_message(SETTING_STRING_VOLTAGE_OUTPUT_LEVEL);
    }
}

output_level_type get_working_voltage_level_setting(void)
{
    return(settings.output_voltage_level);
}

#pragma endregion "voltage level setting support functions"

#pragma region "current level setting support functions"

bool validate_current_level_setting(output_level_type pending_current_level)
{
    bool valid_setting = false;		//init to fail until proved otherwise
    float max_test_output_level = 0;
    float min_test_output_level = 0;
    float test_output_level = 0;
    
    //is the desired combined amplitude and offset greater than the current range I'm presently set to?
    switch(settings.current_range)
    {
        case IRANGE_100mA:
            min_test_output_level = MIN_CURRENT_OUTPUT_100MA_RANGE;
            max_test_output_level = MAX_CURRENT_OUTPUT_100MA_RANGE;
            break;
        
        case IRANGE_10mA:
            min_test_output_level = MIN_CURRENT_OUTPUT_10MA_RANGE;
            max_test_output_level = MAX_CURRENT_OUTPUT_10MA_RANGE;
            break;
        
        case IRANGE_1mA:
            min_test_output_level = MIN_CURRENT_OUTPUT_1MA_RANGE;
            max_test_output_level = MAX_CURRENT_OUTPUT_1MA_RANGE;
            break;
            
        case IRANGE_100uA:
            min_test_output_level = MIN_CURRENT_OUTPUT_100UA_RANGE;
            max_test_output_level = MAX_CURRENT_OUTPUT_100UA_RANGE;
            break;
            
        case IRANGE_10uA:
            min_test_output_level = MIN_CURRENT_OUTPUT_10UA_RANGE;
            max_test_output_level = MAX_CURRENT_OUTPUT_10UA_RANGE;
            break;
           
        case IRANGE_1uA:
        default:
            min_test_output_level = MIN_CURRENT_OUTPUT_1UA_RANGE;
            max_test_output_level = MAX_CURRENT_OUTPUT_1UA_RANGE;
            break;
    }
    
    test_output_level = pending_current_level.amplitude;
    
    if(settings.output_shape != SHAPE_DC)                   //ignore offset for DC current level settings. Only amplitude is used in DC mode.
        test_output_level += pending_current_level.offset;  //Not sure if android will send an offset value when in DC mode. This will protect against that.

    if((test_output_level >= min_test_output_level) && (test_output_level <= max_test_output_level))
    {
        valid_setting = true;
    }
    
    return(valid_setting);
}

void set_current_level_setting(output_level_type validated_current_level, setting_android_notify_type notify_android)
{
    settings.output_current_level.amplitude = validated_current_level.amplitude;
    settings.output_current_level.offset = validated_current_level.offset;
    
    if(settings.output_state_enabled == true && settings.output_mode == CURRENT_MODE)
    {
        if(get_presently_selected_output_stage() == OUTSTG_SEL_LOW_VOLTAGE)
        {
            validated_current_level.amplitude = -validated_current_level.amplitude;
            validated_current_level.offset = -validated_current_level.offset;
        }
        
        if(settings.output_shape == SHAPE_DC)
        {
             update_DAC_output_while_in_DC_mode(validated_current_level.amplitude, get_full_scale_current_range_value(settings.current_range));
        }
        else
        {
            generate_DAC_code_table(validated_current_level.amplitude, validated_current_level.offset, get_full_scale_current_range_value(settings.current_range));
        }   
    }   
    
    if(notify_android == NOTIFY_ANDROID_OF_SETTING_CHANGE)
    {
        generate_local_setting_message(SETTING_STRING_CURRENT_OUTPUT_LEVEL);
    }
}

output_level_type get_working_current_level_setting(void)
{
    return(settings.output_current_level);
}

#pragma endregion "current level setting support functions"

#pragma region "current range setting support functions"
bool validate_current_range_setting(int32_t pending_current_range)
{
	bool valid_setting = false;		//init to fail until proven otherwise
	
	if((pending_current_range >= IRANGE_1uA) && (pending_current_range <= IRANGE_100mA))                            //absolute bounds check
	{
        if((settings.current_compliance_range == I_COMPLIANCE_100V) && (pending_current_range == IRANGE_100mA))    //100V + 100mA is invalid HW setting
        {                                                                                                           //Android should catch this
		    valid_setting = false;
        }      
        else
        {
            valid_setting = true;
        }         
	}
	
	return(valid_setting);
}

void set_current_range_setting(current_range_type validated_current_range, setting_android_notify_type notify_android)
{
    void (*current_range_function_ptr)(current_range_type);
    output_stage_selection_type presently_selected_output_stage;     
         
    current_range_function_ptr = &set_low_voltage_current_range_HW;         //default to low voltage stage to be safe    
    
    if(settings.output_state_enabled == true && settings.output_mode == CURRENT_MODE)
    {
        bring_DAC_output_to_zero(settings.output_shape);
        
        presently_selected_output_stage = get_presently_selected_output_stage();
        
        if(presently_selected_output_stage == OUTSTG_SEL_HIGH_VOLTAGE)                //else, its assumed to be Low Voltage
        {
            current_range_function_ptr = &set_high_voltage_current_range_HW;            
        }
        
        shunt_output_stage(presently_selected_output_stage);
        current_range_function_ptr(validated_current_range);
        unshunt_output_stage(presently_selected_output_stage);    
        execute_output_shape_change_sequence();										 //will apply frequency, output levels, DC vs Sine shape,   
    }        
    
	settings.current_range = validated_current_range;
	
	if(notify_android == NOTIFY_ANDROID_OF_SETTING_CHANGE)
	{
		generate_local_setting_message(SETTING_STRING_CURRENT_RANGE);
	}
}

current_range_type get_working_current_range_setting(void)
{
	return(settings.current_range);
}


#pragma endregion "current range setting support functions"

#pragma region "current autorange enabled setting support functions"

void set_current_autorange_enabled_setting(bool current_autorange_enabled, setting_android_notify_type notify_android)
{
    //This setting is a place holder for future growth. Android takes care of all simple source auto ranging.
    
    settings.current_autorange_enabled = current_autorange_enabled;
    
    if(notify_android == NOTIFY_ANDROID_OF_SETTING_CHANGE)
    {
        generate_local_setting_message(SETTING_STRING_CURRENT_AUTORANGE_ENABLED);
    }
}

bool is_current_autorange_enabled(void)
{
    return(settings.current_autorange_enabled);
}

#pragma endregion "current autorange enabled setting support functions"

#pragma region "current compliance range setting support functions"

void set_current_compliance_range_setting(current_compliance_range_type validated_current_compliance_range, setting_android_notify_type notify_android)
{
    void (*current_range_function_ptr)(current_range_type);
    output_stage_selection_type desired_output_stage = OUTSTG_SEL_LOW_VOLTAGE;    
    current_range_function_ptr = &set_low_voltage_current_range_HW;                 //default to low voltage stage to be safe, in case of corruption           
    
    if(settings.output_state_enabled == true && settings.output_mode == CURRENT_MODE)
    {
        bring_DAC_output_to_zero(settings.output_shape);
        shunt_output_stage(get_presently_selected_output_stage());
        
        if(validated_current_compliance_range == I_COMPLIANCE_100V)                 //else, its assumed to be Low Voltage
        {
            desired_output_stage = OUTSTG_SEL_HIGH_VOLTAGE;
            current_range_function_ptr = &set_high_voltage_current_range_HW;
        }
        
        shunt_output_stage(desired_output_stage);
        configure_DAC_input_to_desired_output_stage(CURRENT_MODE, desired_output_stage);
        current_range_function_ptr(settings.current_range);
        connect_output_stage_to_output_terminals(desired_output_stage);
        unshunt_output_stage(desired_output_stage);
        execute_output_shape_change_sequence();								//will apply frequency, output levels, DC vs Sine shape,        
    }
    
    settings.current_compliance_range = validated_current_compliance_range;  
    
    if(notify_android == NOTIFY_ANDROID_OF_SETTING_CHANGE)
    {
        generate_local_setting_message(SETTING_STRING_CURRENT_COMPLIANCE_RANGE);
    }
}

current_compliance_range_type get_working_current_compliance_range_setting(void)
{
    return(settings.current_compliance_range);
}

#pragma endregion "current compliance range setting support functions"

#pragma region "current compliance status setting support functions"

void set_current_compliance_status_setting(bool current_output_is_in_compliance, setting_android_notify_type notify_android)
{
    settings.current_compliace_active = current_output_is_in_compliance;
    
    if(notify_android == NOTIFY_ANDROID_OF_SETTING_CHANGE)
    {
        generate_local_setting_message(SETTING_STRING_CURRENT_COMPLIANCE_STATUS);
    }
}

bool get_current_output_compliance_status_setting(void)
{
    return(settings.current_compliace_active);
}

#pragma endregion "current compliance status setting support functions"

#pragma region "voltage protection status setting support functions"

void set_voltage_protection_status_setting(bool voltage_output_is_in_current_limit, setting_android_notify_type notify_android)
{    
    settings.voltage_protection_active = voltage_output_is_in_current_limit;
    
    if(notify_android == NOTIFY_ANDROID_OF_SETTING_CHANGE)
    {
        generate_local_setting_message(SETTING_STRING_VOLTAGE_PROTECTION_STATUS);
    }
}

bool get_voltage_output_protection_status_setting(void)
{
    return(settings.voltage_protection_active);
}

#pragma endregion "voltage protection status setting support functions"

#pragma region "terminals setting support functions"
void set_terminals_setting(terminal_selection_type validated_terminals_setting, setting_android_notify_type notify_android)
{
    if(settings.output_state_enabled == true)
    {
        bring_DAC_output_to_zero(settings.output_shape);
        shunt_output_stage(get_presently_selected_output_stage());
        select_front_or_rear_output_terminals(validated_terminals_setting);
        unshunt_output_stage(get_presently_selected_output_stage());
        execute_output_shape_change_sequence();									//will apply frequency, output levels, DC vs Sine shape, 
    }    

    settings.terminal_selection = validated_terminals_setting;
    
    if(notify_android == NOTIFY_ANDROID_OF_SETTING_CHANGE)
    {
        generate_local_setting_message(SETTING_STRING_TERMINALS);
    }
}

terminal_selection_type get_working_terminals_setting(void)
{
    return(settings.terminal_selection);
}

#pragma endregion "terminals setting support functions"

#pragma region "info setting support functions"

void generate_local_info_setting_message(void)
{
    generate_local_setting_message(SETTING_STRING_INFO);
    
}

instrument_info_type get_instrument_info(void)
{
    return(settings.instrument_info);    
}

#pragma endregion "info setting support functions"

#pragma region "CalData setting support functions"

calibration_data_type get_calibration_data(void)
{
    return(get_caldata_values_in_RAM());
}

void set_calibraton_data(calibration_data_type incoming_calibration_data, setting_android_notify_type notify_android)
{
    update_caldata_values_in_RAM(incoming_calibration_data);
    
    if(notify_android == NOTIFY_ANDROID_OF_SETTING_CHANGE)
    {
        generate_local_setting_message(SETTING_STRING_CALDATA);
    }
}

void generate_local_caldata_setting_message(void)
{
    generate_local_setting_message(SETTING_STRING_CALDATA);
}

#pragma endregion "CalData setting support functions"