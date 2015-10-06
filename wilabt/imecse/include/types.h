#ifndef TYPES_H
#define TYPES_H

/*************************************************************************
 * Design Units: types definition
 *
 * File name	: types.h
 *
 * Purpose	: include-file that defines the different types used for DIFFS-control
 *
 * Initial date: 2010-10-19
 *
 * Errors 	:
 *
 * Library	:
 *
 * Dependencies:
 *
 * Author 	: Mattias Desmet
 *
 *	Copyright IMEC SSET SITEX
 *
 *------------------------------------------------------------------------
 * Notes: 
 *
 *------------------------------------------------------------------------
 * History:
 * 2010-10-19: Initial version
************************************************************************/

#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <stdint.h>
#include <math.h>
#include <string.h>
// #include <pthread.h>

/**********************************
         TYPE DEFINITIONS
**********************************/

/*
 * Basic types
 */

// typedef enum { false, true } boolean;
typedef int boolean;
#define false 0
#define true 1

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
# if __WORDSIZE == 64
typedef unsigned long int uint64_t;
#endif
typedef signed short int16_t;

#endif /* TYPES_H */
