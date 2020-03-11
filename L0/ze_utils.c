#include <stdio.h>
#include <stdlib.h>
#include "ze_api.h"
#include "./ze_utils.h"

//  _
// |_)  _  |  _  ._ ._  |  _. _|_  _
// |_) (_) | (/_ |  |_) | (_|  |_ (/_
//                  |

/* - - - -
IO
- - - - */
int read_from_binary(unsigned char **output, size_t *size, const char *name) {
  FILE *fp = fopen(name, "rb");
  if (!fp) {
    return -1;
  }

  fseek(fp, 0, SEEK_END);
  *size = ftell(fp);
  fseek(fp, 0, SEEK_SET);

  *output = (unsigned char *)malloc(*size * sizeof(unsigned char));
  if (!*output) {
    fclose(fp);
    return -1;
  }

  fread(*output, *size, 1, fp);
  fclose(fp);
  return 0;
}

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

