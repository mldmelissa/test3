/*  @file output_control.h
 *  @brief interface definition for the output control module
 *  
 *  The primary purpose of this module is to define the functions needed by the application to configure and control the
 *  circuitry needed to generate an output waveform.
 *  
 *  This includes the functions to manipulate the DAC, configure gain stages, output stage behavior (voltage or current output). Basically
 *  everything needed to generate, manipulate, and route the output signal to the respective output terminals.
 *  
 *    
 *  @author Adam Porsch & Houston Fortney
 *  @bug No known bugs.
 */

#ifndef OUTPUT_CONTROL_H_
#define OUTPUT_CONTROL_H_


#include "types.h"
#include "sine_wave.h"
#include "settings_manager.h"

typedef enum{OUTSTG_SEL_BOTH, OUTSTG_SEL_LOW_VOLTAGE, OUTSTG_SEL_HIGH_VOLTAGE}output_stage_selection_type;

//------------------------- General Output Control Function Prototypes ------------------------- 
void initialize_output_control_parameters(void);

//------------------------- AD5791 DAC Manipulation Function Prototypes ------------------------- 
void init_AD5791_DAC(void);
/**
 * \brief calculates properly scaled and calibrated DAC codes used by the ISR for waveform generation
 * 
 * \param amplitude desired output signal pk value. Only provided to this function while shape when in AC mode.
 * \param offset the user's amplitude setting when in DC mode, the user's offset setting when in AC mode.
 * \param full_scale_divisor The nominal full scale divisor of the active output stage, in base units. For example, 
 *                           1V range is selected, this value is 1.0V
 *                           
 *  To the 155 user, driven by SCPI and .NET requirements, the "amplitude" setting represents the peak output value in AC mode and 
 *  the steady state output value in DC mode. Offset therefore, to the user, is only used in AC mode and is not applicable in DC mode.
 *  Unfortunately, this doesn't jive with the idea of using this same routine for both AC and DC DAC code generation.
 *  
 *  Therefore, when this application code needs to generate DC DAC codes, it calls this function by passing the "amplitude" user 
 *  setting value into the "offset" parameter of this function. We'll then we'll end up with the desired result since the amplitude
 *  component will be nulled out, thus nulling out the normalized sine wave component of the calculation as well.
 *  
 *  In addition, this function was implemented using the sin() function as provided by math.h. It was observed that it would take
 *  150mS to calculate a 4096 point DAC code table. However, using a const normalized sine wave table, it only would take 7mS to execute.
 *  Therefore, for the sake of responsiveness, as this function is inherently called every time the output frequency, amplitude, or offset
 *  is changed, it was decided to move forward with the const sine wave table.
 * 
 * \return void
 */
void generate_DAC_code_table(float amplitude, float offset, float full_scale_divisor);

/**
 * \brief Brings the DAC output to zero.
 * 
 * Typically used by application when HW is being reconfigured and we want to bring the 
 * output stages to a safe, low power state when doing so.
 * 
 * \param output_shape Used by function to determine how to go about bringing the DAC output to zero.
 * 
 * \return void
 */
void bring_DAC_output_to_zero(output_shape_type output_shape);

/**
 * \brief 
 * 
 * \param desired_output_amplitude user amplitude setting.
 * \param full_scale_divisor FS divisor determined by what full scale range the output stage is configured for
 * 
 * \return void
 */
void update_DAC_output_while_in_DC_mode(float desired_output_amplitude, float full_scale_divisor);


//------------------------- Output Stage Hardware Manipulation Function Prototypes ------------------------- 
/**
 * @brief Enables LV or HV stage and configures it to V or I mode
 * 
 * Inherently, it will disable the DAC input to the other stage since we don't want both stages driven simultaneously.
 * 
 * Note, when the desired output stage is to OUTSTG_SEL_BOTH, the output mode is consumed as a don't care
 * and is used to set A0 of the ADG409 to a known state. In addition, since the DAC should not be actively 
 * fed to both output stages simultaneously, OUTSTG_SEL_BOTH will cause both ADG409s of the respective output
 * stages to "zero-out" the DAC input.
 * 
 * @param output_mode Either voltage or current
 * @param desired_output_stage Either low voltage or high voltage, or both disabled.
 * 
 * @return void
 */
void configure_DAC_input_to_desired_output_stage(output_mode_type output_mode, output_stage_selection_type desired_output_stage);

/**
 * \brief returns value indicating which output stage is selected
 * 
 * "Selected" is defined as the ADG409 of the output stage in question is configured to
 *  allow the DAC output signal to pass into the output stage.
 * 
 * \param 
 * 
 * \return output_stage_selection_type returns state of output stages
 */
output_stage_selection_type get_presently_selected_output_stage(void);

/**
 * \brief configures the ADG409 to determine the full scale output voltage used in the low voltage output stage
 * 
 * \param range desired full scale output voltage
 * 
 * \return void
 */
void set_low_voltage_voltage_range_HW(voltage_range_type range);

/**
 * \brief Selects the current shunt resistor in the low voltage output stage.
 * 
 * When the output mode is current, the shunt resistor is used to determine the full scale current output
 * When the output mode is voltage, the shunt resistor is used to determine the full scale current limit of the 
 * voltage output.
 * 
 * \param range the desired full scale current range
 * 
 * Note, there is a special case where this function can be called. When in AC VOLTAGE mode, the LV stage current range
 * needs to be set to 100mA. This ensures the minimum amount of series resistance for the voltage output.
 * 
 * \return void
 */
void set_low_voltage_current_range_HW(current_range_type range);

/**
 * \brief Selects the current shunt resistor in the high voltage output stage.
 * 
 * When the output mode is current, the shunt resistor is used to determine the full scale current output
 * When the output mode is voltage, the shunt resistor is used to determine the full scale current limit of the
 * voltage output.
 * 
 * \param range the desired full scale current range
 * 
 * Note, there is a special case where this function can be called. When in AC VOLTAGE mode, the HV stage current range
 * needs to be set to AC_BYPASS. This ensures the minimum amount of series resistance for the voltage output. Unlike the 
 * LV output stage which has 10 ohms series resistance when in AC V mode, the lowest shunt resistor in the High Voltage
 * stage is 1k. Therefore, this stage has an option to completely bypass the shunt resistors all together.
 * 
 * \return void
 */
void set_high_voltage_current_range_HW(current_range_type range);


/**
 * @brief shunts either the LV, HV or both output stages
 * 
 * @param output_stage enum to determine what to shunt
 * 
 * @return void
 */
void shunt_output_stage(output_stage_selection_type output_stage);

/**
 * @brief unshunts the specified output stage
 * 
 * Passing in OUTSTG_SEL_BOTH has no effect since both stages can't be un-shunted at the same time.
 * This would imply they would be active at he same time.
 * 
 * @param output_stage enum to determine what to un-shunt
 * 
 * @return void
 */
void unshunt_output_stage(output_stage_selection_type output_stage);

/**
 * @brief routes the specified output stage to the output terminal selection relays
 * 
 * Passing in OUTSTG_SEL_BOTH has no effect since both stages can't be passed to
 * the output terminals at the same time.
 * 
 * @param output_stage
 * 
 * @return void
 */
void connect_output_stage_to_output_terminals(output_stage_selection_type output_stage);

/**
 * @brief disables BOTH the low voltage and high voltage output stages from the
 * presently select output terminals.
 * 
 * The result is the connection is broken between both stages and the output,
 * causing the output terminals to float.
 * 
 * @param none
 * 
 * @return void
 */
void disable_output_terminals(void);

/**
 * @brief configures the relays to pass the output stage signal to either front or rear
 * 
 * @param terminal_selection enum specifying front or rear
 * 
 * @return void
 */
void select_front_or_rear_output_terminals(terminal_selection_type terminal_selection);

//------------------------- Output Waveform Function Prototypes ------------------------- 
/**
 * \brief updates the "phase increment" variable used by the timer ISR for waveform generation
 * 
 * Phase increment eventually translates into the sine_table delta (i.e. how many indices to increment 
 * by with every firing of the ISR).
 * 
 * \param desired_output_frequency desired user output frequency
 * 
 * \return void
 */
void set_output_frequency(float desired_output_frequency);

/**
 * \brief Executes sequence needed to update timers/firmware to output either AC or DC signal
 *  * 
 * \param desired_output_shape DC or SINE
 * \param amplitude user amplitude setting
 * \param offset user offset setting
 * \param full_scale_divisor full scale current or voltage range of active stage
 * 
 * \return void
 */
void set_output_shape(output_shape_type desired_output_shape, float amplitude, float offset, float full_scale_divisor);

#endif /* OUTPUT_CONTROL_H_ */
