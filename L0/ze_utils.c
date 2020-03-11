#include <stdio.h>
#include <stdlib.h>
#include "ze_api.h"
#include "./ze_utils.h"

/* - - - -
OpenCL Error
- - - - */

const char *getErrorString(ze_result_t error)
{
switch(error){
    // run-time and JIT compiler errors

    default: return "Unknown L0 error";
 }
}

void check_error(ze_result_t error, char const *name) {
    if (error != ZE_RESULT_SUCCESS) {
        fprintf(stderr, "Non-successful return code %d (%s) for %s.  Exiting.\n", error, getErrorString(error), name);
        exit(EXIT_FAILURE);
    }
}

void exit_msg(char const *str){
        fprintf(stderr, "%s \n", str);
        exit(EXIT_FAILURE);
}

