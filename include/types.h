/*
 * types.h
 *
 * Created: 9/25/2015 10:52:28 AM
 *  Author: houston.fortney
 */ 

#ifndef TYPES_H_
#define TYPES_H_

union four_byte_pack {
  long int packed;
  unsigned char bytes[4];
};

#endif
