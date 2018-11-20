
#define pragama once

#include<stdint.h>
#include <fstream>
#include <iostream>
#include <string>
#include "sensor.h"
#include <map>
#include <cstring>
#include <thread>
#include <mutex>
#include <unistd.h>

#define COUNTER_MAX_FOR_5S 50

#define EMULATE_FLAG 1

//#define DEBUG

//#define TRACE_ENABLE

/** DEBUG and PRINT MACRO for extendablity*/
#ifdef DEBUG
#define DEBUG_PRINT(fmt, args...)    fprintf(stderr, fmt, ## args)
#else
#define DEBUG_PRINT(fmt, args...)
#endif

#define ERROR_PRINT(fmt, args...)    fprintf(stderr, fmt, ## args)

#ifdef TRACE_ENABLE
#define TRACE_PRINT(fmt, args...)    fprintf(stderr, fmt, ## args)
#else
#define TRACE_PRINT(fmt, args...)
#endif

/** Boiler Plate Macros of NULL CHECK RETURN for code reuse and Reablity*/
#define RETURN_IF_NULL(arg, msg)   do{ if((arg) == NULL) { \
                ERROR_PRINT("%s: Caught null pointer for parameter %s\r\n", \
                __func__, (msg));\
                 return SENSOR_INVALID_PARAMETER;\
                }} while(0)


#define RETURN_ON_FAILURE(arg, msg)   do{ if((arg) > SENSOR_SUCESS) { \
                ERROR_PRINT(" In: %s Code: %d Message: %s \r\n", \
                __func__, arg, (msg));\
                return SENSOR_FAILURE;\
                }} while(0)

#define DEBUGTRACE_ENTRY()  TRACE_PRINT("entering  %s   \r\n", __func__)
#define DEBUGTRACE_EXIT()  TRACE_PRINT("exiting  %s   \r\n", __func__)

/* error Code */
#define SENSOR_SUCESS				0
#define SENSOR_INVALID_PARAMETER	1
#define SENSOR_INTERNAL_ERROR		2
#define SENSOR_API_ERROR			3
#define SENSOR_FAILURE				4

int start_emulator();