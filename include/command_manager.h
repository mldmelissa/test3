/** @file command_manager.h
 *  @brief Header file for the simple sources command manager implementation
 *    
 *  This module acts as an interface with the sources_command_callbacks module to allow the command callback
 *  routines to call simple sources specific command execution routines.
 *  
 *  For use cases where the embedded ARM needs to act as a "main" micro (client), the check_status_of_input_micro() command
 *  is defined here as an example of how the application will issue a remote command to an "input" micro (server), 
 *  block for a response, and retrieve the returned data that the remote command response callback is responsible for
 *  populating.
 *  
 *  @author Adam Porsch
 *  @bug No known bugs.
 */

#ifndef COMMAND_MANAGER_H_
#define COMMAND_MANAGER_H_

#include "sam.h"

/**
 * @brief executes an incoming request to read a byte of the local EEPROM
 * 
 * this function is invoked by the LSCP local command and associated response message callback
 * when a remote client (the android board) wants to read a byte out of local EEPROM.
 * 
 * @param address the 32-bit address of EEPROM memory to read from
 * 
 * @return uint8_t the byte contained in memory space
 */
uint8_t execute_ReadEEPROM_command(uint32_t address);

/**
 * @brief executes an incoming request to return the simple sources version info
 * 
 * this function is invoked by the LSCP local command and associated response message callback
 * when a remote client (the android board) requests version info from the embedded ARM.
 * 
 * Note, this command is going to change as the version info requirements are refined throughout development.
 * 
 * @param the version info command has no incoming data parameters.
 * 
 * @return const char ** 2D string array containg the version info
 * 
 * Example:
 * version_info_string[] = {"LSA1236",			//serial number
 *							"122",				//Model number
 *							"1.0",				//Firmware Revision
 *							"1"}				//Board revision
 *							
 */
const char **execute_VersionInfo_command(void);

/**
 * @brief called by application when it wants to request the status from an input micro
 * 
 * This function is here here to provide an example of how the embedded ARM code would
 * consume the LSCP library to issue a remote command, wait for, and then consume
 * the response. 
 * 
 * @param input_micro_number integer enum of input micros (assuming multiple input micros)
 * 
 * @return uint32_t input micro status, likely some kind of enumeration or bit encoded value
 */
uint32_t check_status_of_input_micro(uint32_t input_micro_number);


#endif /* COMMAND_MANAGER_H_ */