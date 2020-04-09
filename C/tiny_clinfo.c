// Includes
#include <stdio.h>
#include <CL/cl.h>
#include "./cl_utils.h"

#define CL_DEVICE_SLICE_COUNT_INTEL 0x10020
#define CL_QUEUE_SLICE_COUNT_INTEL 0x10021

int main(int argc, char* argv[]) {

   cl_int err;

   //  _              _                      _
   // |_) |  _. _|_ _|_ _  ._ ._ _    ()    | \  _     o  _  _
   // |   | (_|  |_  | (_) |  | | |   (_X   |_/ (/_ \/ | (_ (/_
   //
    printf(">>> Initializing OpenCL Platform and Device...\n");

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
    for (cl_uint platform_idx=0; platform_idx < platform_count ; platform_idx++) {
        char name[128];
        cl_platform_id platform = platforms[platform_idx];
        err = clGetPlatformInfo(platform, CL_PLATFORM_NAME, 128, name, NULL);
        check_error(err, "clGetPlatformInfo");
        printf("Platform #%d: %s\n", platform_idx, name);

        /* - - - -
        Device
        - - - - */
        // Device gather data         
        cl_uint device_count;
        err = clGetDeviceIDs(platform,  CL_DEVICE_TYPE_ALL , 0, NULL, &device_count);
        check_error(err, "clGetdeviceIds");

        cl_device_id* devices = (cl_device_id*)malloc(sizeof(cl_device_id) * device_count);
        err = clGetDeviceIDs(platform,  CL_DEVICE_TYPE_ALL , device_count, devices, NULL);
        check_error(err, "clGetdeviceIds");

        // Device print
        for (cl_uint device_idx=0; device_idx < device_count ; device_idx++) {
            cl_device_id device = devices[device_idx];
            size_t  numSlices  = 0;
            err = clGetDeviceInfo(device, CL_DEVICE_SLICE_COUNT_INTEL, sizeof(numSlices), &numSlices, NULL);
            check_error(err, "clGetPlatformInfo");
            printf("-- Device Number of slice #%d: %lu\n", device_idx, numSlices);
         }
    }
}
// =================================================================================================
