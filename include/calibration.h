/*
 * calibration.h
 *
 * Created: 4/26/2016 11:04:00 AM
 *  Author: Adam.Porsch
 */ 


#ifndef CALIBRATION_H_
#define CALIBRATION_H_

//TODO: Update cal struct definitions
//4/26/16 This definition is a placeholder to adhere to the LSCP CalData packet definition to get up and running. 
//I'll likely need to redefine the struct to better reflect the HW

#define NUMBER_OF_VOLTAGE_CAL_POINTS                            5
#define NUMBER_OF_CURRENT_CAL_POINTS                            11
#define MAX_NUMBER_OF_GAINS_AND_OFFSETS_FOR_AN_OUTPUT           NUMBER_OF_CURRENT_CAL_POINTS     



typedef struct
{
    float gains[MAX_NUMBER_OF_GAINS_AND_OFFSETS_FOR_AN_OUTPUT];
    float offsets[MAX_NUMBER_OF_GAINS_AND_OFFSETS_FOR_AN_OUTPUT];
        
}calibration_set_type;


typedef struct  
{
    const char *serial_number;              //TODO: define as writeable char array
    bool ac_enabled;
    const char *date;                       //TODO: define as writeable char array
    const char *due_date;                   //TODO: define as writeable char array
    calibration_set_type current;
    calibration_set_type voltage;
    
}calibration_data_type;

extern calibration_data_type calibration_data;         //TODO: REMOVE, JUST HERE FOR INITIAL INTEGRATION TESTING
 

void load_calibration_default_values(void);

calibration_data_type get_caldata_values_in_RAM(void);
void update_caldata_values_in_RAM(calibration_data_type udpated_calibration_data);


#endif /* CALIBRATION_H_ */