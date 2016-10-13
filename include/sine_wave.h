/**
 * @file sine_wave.h
 *
 * @brief This module implements Direct Digital Synthesis (DDS) of a sine wave.
 * It operates on a table of size specified as 2^SINE_TABLE_BITS which can be set by the user at compile time. The index is augmented
 * by an oversized accumulator whose size is also specified at compile time. This oversized accumulator allows for output frequencies that 
 * are not integer factors of the table itself.
 *
 */

#ifndef SINE_WAVE_H_
#define SINE_WAVE_H_

#define SINE_TABLE_BITS 12				//2^SINE_TABLE_BITS = the number of 32-bit floating point values that comprise the normalized sine wave table (SINE_TABLE_SIZE)	
#define SINE_TABLE_OVERSIZE_BITS 16		//These upper 16-bits of the accumulator are used to determine which point, at a particular instance in time of the timer ISR firing, will be fetched from the sine table.
										//Because the sampling frequency is fixed, the time delay required to reach a given table index is controlled by the resolution of the accumulator.

#define SINE_TABLE_SIZE (1 << SINE_TABLE_BITS)
#define PERIOD_COUNTS (1 << (SINE_TABLE_BITS + SINE_TABLE_OVERSIZE_BITS))			//the total number of counts representing 1 full cycle of the normalized sine table

/**
 * @brief Computes index into sine table from oversized phase accumulator.
 * 
 * @param accumulator Oversized phase accumulator
 * 
 * @return unsigned int Limited index appropriate for table lookup
 */
#define TABLE_INDEX(accumulator) (((accumulator) >> SINE_TABLE_OVERSIZE_BITS) & (SINE_TABLE_SIZE - 1))

extern const float sine_table[SINE_TABLE_SIZE];

/**
 * @brief Determines the phase increment required to move through the sine table at the desired output frequency.
 * 
 * @param frequency The desired frequency of the output sine wave
 * @param sampling_frequency The frequency with which the phase accumulator is updated with this increment
 * 
 * @return unsigned int Phase increment The increment count value used to traverse through the sine table from sample to sample
 */
unsigned int get_phase_increment(float frequency, float sampling_frequency);

#endif // Guard block