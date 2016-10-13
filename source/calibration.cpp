/*
 * calibration.cpp
 *
 * Created: 4/26/2016 11:04:12 AM
 *  Author: Adam.Porsch
 */ 


#include "calibration.h"

calibration_data_type calibration_data;

void load_calibration_default_values(void)
{
    unsigned int counter = 0;
    
    calibration_data.serial_number = "Prototype";
    calibration_data.ac_enabled = true;
    calibration_data.date = "2017-01-01T00:30:00Z";
    calibration_data.due_date = "2018-01-01T00:30:00Z";
    
    //voltages
    for(counter = 0; counter < NUMBER_OF_VOLTAGE_CAL_POINTS; counter++)
    {
        calibration_data.voltage.gains[counter] = 1.0;
        calibration_data.voltage.offsets[counter] = 0.0;
    }
    
    //currents
    for(counter = 0; counter < NUMBER_OF_CURRENT_CAL_POINTS; counter++)
    {
        calibration_data.current.gains[counter] = 1.0;
        calibration_data.current.offsets[counter] = 0.0;
    }
}

void update_caldata_values_in_RAM(calibration_data_type udpated_calibration_data)
{
    calibration_data = udpated_calibration_data;
}

calibration_data_type get_caldata_values_in_RAM(void)
{
    return(calibration_data);
}
