/** @file settings_manager.h
 *  @brief Header file for the simple sources setting manager implementation
 *    
 *  This module acts as an interface with the sources_settings_callbacks module to allow the setting callback
 *  routines to call simple sources specific validation, get and set routines.
 *  
 *  It should also be noticed that the set_xxxx() functions in this module have a function paramter of type
 *  setting_android_notify_type. This is to handle two scenarios, using set_voltage_range as an example:
 *  1) The simple sources application code has updated the voltage range by calling set_voltage range().
 *     In this case, the android needs to be notified of this change since it did not explicitly request it. Therefore
 *     the application would make the call to set_voltage_range() and pass in NOTIFY_ANDROID_OF_SETTING_CHANGE to the function.
 *     The generate_local_setting_message() function will then be called and the android will be notified.
 *  2) The android board has sent the embedded ARM a new voltage range setting.
 *     In this case, the LSCP library will invoke the local setting message callback to process the new incoming voltage range setting.
 *     Assuming validation passes, that callback, local_setting_msg_cb_voltage_range, makes a call into set_voltage_range().
 *     However, when the callback returns execution to the LSCP library, LSCP makes the decision if a setting response message
 *     needs to be generated and sent to the android board, based on the ID field being present or not.
 *     Without the condition, set_voltage_range() would always send a setting message to the Android, even if
 *     the Android board did not request it.
 *  
 *  @author Adam Porsch
 *  @bug No known bugs.
 */

#ifndef SETTINGS_MANAGER_H_
#define SETTINGS_MANAGER_H_

#include "calibration.h"

typedef enum {VRANGE_10mV = 1, VRANGE_100mV, VRANGE_1V, VRANGE_10V, VRANGE_100V} voltage_range_type;
typedef enum {IRANGE_1uA = 1, IRANGE_10uA, IRANGE_100uA, IRANGE_1mA, IRANGE_10mA, IRANGE_100mA, IRANGE_HV_AC_BYPASS} current_range_type;
typedef enum {VOLTAGE_MODE = 0, CURRENT_MODE} output_mode_type;
typedef enum {SHAPE_DC = 0, SHAPE_SINE} output_shape_type;
typedef enum {I_COMPLIANCE_10V = 1, I_COMPLIANCE_100V} current_compliance_range_type;
typedef enum {TERMINALS_FRONT = 0, TERMINALS_REAR} terminal_selection_type;

typedef struct  
{
    float amplitude;
    float offset;    
}output_level_type;

typedef struct
{
    const char *model_number;		// Human readable string for displaying the instrument model number
	const char *firmware_type;		// Human readable string denoting the firmware target instrument (113-xxx); primary usage for package management
    const char *version_name;
    const char *board_revision;
    bool current_capability;
}instrument_info_type;

typedef struct  
{
    output_mode_type output_mode;
    bool output_state_enabled;
    bool calibration_locked;
    float output_frequency;
    output_shape_type output_shape;
    voltage_range_type voltage_range;
    bool voltage_autorange_enabled;
    output_level_type output_voltage_level;
    output_level_type output_current_level;
    current_range_type current_range;
    bool current_autorange_enabled;
    current_compliance_range_type current_compliance_range;
    bool current_compliace_active;
    bool voltage_protection_active;  
    terminal_selection_type terminal_selection;  
    instrument_info_type instrument_info;    
}settings_type;

typedef enum {NOTIFY_ANDROID_OF_SETTING_CHANGE, DO_NOT_NOTIFY_ANDROID_OF_SETTING_CHANGE} setting_android_notify_type;
	
/*
 * All functions to manipulate the settings are named using a "set" prefix. 
 * All functions to retrieve settings have a "get" prefix.
 */
void load_settings_struct_with_default_values(void);

void test_init_function(void);          //TODO: REMOVE!!!!!!!!!!!!!!!

//------------------------- general setting manager function prototypes ------------------------- 
void execute_start_command(void);
bool simple_validate_setting(int32_t value_to_validate, int32_t low_test_value, int32_t high_test_value);
void execute_disable_output_sequence(void);
void execute_enable_output_sequence(void);
float get_full_scale_voltage_range_value(voltage_range_type voltage_range);
float get_full_scale_current_range_value(current_range_type current_range);

//------------------------- mode setting function prototypes ------------------------- 
void set_mode_setting(output_mode_type validated_mode, setting_android_notify_type notify_android);
output_mode_type get_working_mode_setting(void);

//------------------------- output state setting function prototypes -------------------------
void set_output_state_enabled_setting(bool enable_output, setting_android_notify_type notify_android);
bool is_output_state_enabled(void);

//------------------------- CalLocked setting function prototypes -------------------------
void set_calibration_locked_setting(bool calibration_locked, setting_android_notify_type notify_android);
bool is_calibration_locked(void);

//------------------------- frequency setting function prototypes ------------------------- 
#define MINIMUM_FREQUENCY_IN_HZ			0.100
#define MAXIMUM_FREQUENCY_IN_HZ			100000.0
/**
 * @brief validates the requested frequency setting from the remote interface
 * 
 * @param pending_frequency the desired output frequency
 * 
 * @return bool false = validation failed, true = validation passed
 */
bool validate_frequency_setting(float pending_frequency);
void set_frequency_setting(float validated_frequency, setting_android_notify_type notify_android);
float get_working_frequency_setting(void);

//------------------------- shape setting function prototypes ------------------------- 
void execute_output_shape_change_sequence(void);
void set_shape_setting(output_shape_type validated_shape, setting_android_notify_type notify_android);
output_shape_type get_working_shape_setting(void);

//------------------------- voltage range setting function prototypes ------------------------- 
void set_voltage_range_setting(voltage_range_type validated_voltage_range, setting_android_notify_type notify_android);
voltage_range_type get_working_voltage_range_setting(void);

//------------------------- voltage autorange setting function prototypes ------------------------- 
void set_voltage_autorange_setting(bool voltage_autorange_enabled, setting_android_notify_type notify_android);
bool is_voltage_autorange_enabled(void);

//------------------------- voltage level setting function prototypes ------------------------- 
#define MAX_VOLTAGE_OUTPUT_100V_RANGE           100.0
#define MIN_VOLTAGE_OUTPUT_100V_RANGE          -100.0
#define MAX_VOLTAGE_OUTPUT_10V_RANGE            10.0
#define MIN_VOLTAGE_OUTPUT_10V_RANGE           -10.0 
#define MAX_VOLTAGE_OUTPUT_1V_RANGE             1.0
#define MIN_VOLTAGE_OUTPUT_1V_RANGE            -1.0 
#define MAX_VOLTAGE_OUTPUT_100MV_RANGE          0.100
#define MIN_VOLTAGE_OUTPUT_100MV_RANGE         -0.100
#define MAX_VOLTAGE_OUTPUT_10MV_RANGE           0.010
#define MIN_VOLTAGE_OUTPUT_10MV_RANGE          -0.010

bool validate_voltage_level_setting(output_level_type pending_voltage_level);
void set_voltage_level_setting(output_level_type validated_voltage_level, setting_android_notify_type notify_android);
output_level_type get_working_voltage_level_setting(void);

//------------------------- current level setting function prototypes ------------------------- 
#define MAX_CURRENT_OUTPUT_100MA_RANGE          0.100
#define MIN_CURRENT_OUTPUT_100MA_RANGE         -0.100
#define MAX_CURRENT_OUTPUT_10MA_RANGE           0.010
#define MIN_CURRENT_OUTPUT_10MA_RANGE          -0.010
#define MAX_CURRENT_OUTPUT_1MA_RANGE            0.001
#define MIN_CURRENT_OUTPUT_1MA_RANGE           -0.001
#define MAX_CURRENT_OUTPUT_100UA_RANGE          0.0001
#define MIN_CURRENT_OUTPUT_100UA_RANGE         -0.0001
#define MAX_CURRENT_OUTPUT_10UA_RANGE           0.00001
#define MIN_CURRENT_OUTPUT_10UA_RANGE          -0.00001
#define MAX_CURRENT_OUTPUT_1UA_RANGE            0.000001
#define MIN_CURRENT_OUTPUT_1UA_RANGE           -0.000001



bool validate_current_level_setting(output_level_type pending_current_level);
void set_current_level_setting(output_level_type validated_current_level, setting_android_notify_type notify_android);
output_level_type get_working_current_level_setting(void);

//------------------------- current range setting function prototypes ------------------------- 
/**
 * @brief validates the requested current range from the remote interface
 * 
 * @param pending_current_ranzge the desired current range
 * 
 * @return bool false = validation failed, true = validation passed
 */
bool validate_current_range_setting(int32_t pending_current_range);
void set_current_range_setting(current_range_type validated_current_range, setting_android_notify_type notify_android);
current_range_type get_working_current_range_setting(void);

//------------------------- current autorange enabled setting function prototypes -------------------------
void set_current_autorange_enabled_setting(bool current_autorange_enabled, setting_android_notify_type notify_android);
bool is_current_autorange_enabled(void);

//------------------------- current compliance range setting function prototypes ------------------------- 
void set_current_compliance_range_setting(current_compliance_range_type validated_current_compliance_range, setting_android_notify_type notify_android);
current_compliance_range_type get_working_current_compliance_range_setting(void);

//------------------------- current compliance status setting function prototypes -------------------------
void set_current_compliance_status_setting(bool current_output_is_in_compliance, setting_android_notify_type notify_android);
bool get_current_output_compliance_status_setting(void);

//------------------------- voltage protection status setting function prototypes -------------------------
void set_voltage_protection_status_setting(bool voltage_output_is_in_current_limit, setting_android_notify_type notify_android);
bool get_voltage_output_protection_status_setting(void);

//------------------------- terminals setting function prototypes -------------------------
void set_terminals_setting(terminal_selection_type validated_terminals_setting, setting_android_notify_type notify_android);
terminal_selection_type get_working_terminals_setting(void);

//------------------------- info setting function prototypes -------------------------
void generate_local_info_setting_message(void);
instrument_info_type get_instrument_info(void);

//------------------------- CalData setting function prototypes -------------------------
calibration_data_type get_calibration_data(void);
void set_calibraton_data(calibration_data_type incoming_calibration_data, setting_android_notify_type notify_android);
void generate_local_caldata_setting_message(void);

//piggy back off the #defines for now since max voltage/currents and full scale voltage/currents are the same
#define FULL_SCALE_100V_RANGE_VALUE     MAX_VOLTAGE_OUTPUT_100V_RANGE
#define FULL_SCALE_10V_RANGE_VALUE      MAX_VOLTAGE_OUTPUT_10V_RANGE
#define FULL_SCALE_1V_RANGE_VALUE       MAX_VOLTAGE_OUTPUT_1V_RANGE
#define FULL_SCALE_100MV_RANGE_VALUE    MAX_VOLTAGE_OUTPUT_100MV_RANGE
#define FULL_SCALE_10MV_RANGE_VALUE     MAX_VOLTAGE_OUTPUT_10MV_RANGE

#define FULL_SCALE_100MA_RANGE_VALUE    MAX_CURRENT_OUTPUT_100MA_RANGE
#define FULL_SCALE_10MA_RANGE_VALUE     MAX_CURRENT_OUTPUT_10MA_RANGE 
#define FULL_SCALE_1MA_RANGE_VALUE      MAX_CURRENT_OUTPUT_1MA_RANGE   
#define FULL_SCALE_100UA_RANGE_VALUE    MAX_CURRENT_OUTPUT_100UA_RANGE 
#define FULL_SCALE_10UA_RANGE_VALUE     MAX_CURRENT_OUTPUT_10UA_RANGE  
#define FULL_SCALE_1UA_RANGE_VALUE      MAX_CURRENT_OUTPUT_1UA_RANGE       


#endif /* SETTINGS_MANAGER_H_ */