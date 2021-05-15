// Includes
#include <stdio.h>
#include <CL/opencl.h>
#include "./cl_utils.h"

int main() {

   cl_int err;

   //  _              _            
   // |_) |  _. _|_ _|_ _  ._ ._ _  
   // |   | (_|  |_  | (_) |  | | |
   //
    printf(">>> Initializing OpenCL Platform...\n");

    /* - - - -
    Plateform
    - - - - */
    //A platform is a specific OpenCL implementation, for instance AMD, NVIDIA or Intel.
    // Intel may have a different OpenCL implementation for the CPU and GPU.

    // Discover the number of platforms:
    cl_uint platform_count;
    err = clGetPlatformIDs(0, NULL, &platform_count);
    check_error(err, "clGetPlatformIds");

    // Now ask OpenCL for the platform IDs:
    cl_platform_id* platforms = (cl_platform_id*)malloc(sizeof(cl_platform_id) * platform_count);
    err = clGetPlatformIDs(platform_count, platforms, NULL);
    check_error(err, "clGetPlatformIds");

    //Print information on the platform IDs (hardcoded maximun size of message for briefty):
    cl_uint platform_idx;  cl_platform_id platform;
    for (platform_idx=0; platform_idx < platform_count ; platform_idx++) {
        char name[128];
        platform = platforms[platform_idx];
        err = clGetPlatformInfo(platform, CL_PLATFORM_NAME, 128, name, NULL);
        check_error(err, "clGetPlatformInfo");
        printf("Platform %d: %s\n", platform_idx, name);
    }
}

// =================================================================================================
