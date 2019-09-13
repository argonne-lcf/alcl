// Includes
#include <stdio.h>
#include <CL/cl.h>

//  _
// |_)  _  |  _  ._ ._  |  _. _|_  _
// |_) (_) | (/_ |  |_) | (_|  |_ (/_
//                  |

/* - - - -
OpenCL Error
- - - - */

const char *getErrorString(cl_int error)
{
switch(error){
    // run-time and JIT compiler errors
    case 0: return "CL_SUCCESS";
    case -1: return "CL_DEVICE_NOT_FOUND";
    case -2: return "CL_DEVICE_NOT_AVAILABLE";
    case -3: return "CL_COMPILER_NOT_AVAILABLE";
    case -4: return "CL_MEM_OBJECT_ALLOCATION_FAILURE";
    case -5: return "CL_OUT_OF_RESOURCES";
    case -6: return "CL_OUT_OF_HOST_MEMORY";
    case -7: return "CL_PROFILING_INFO_NOT_AVAILABLE";
    case -8: return "CL_MEM_COPY_OVERLAP";
    case -9: return "CL_IMAGE_FORMAT_MISMATCH";
    case -10: return "CL_IMAGE_FORMAT_NOT_SUPPORTED";
    case -11: return "CL_BUILD_PROGRAM_FAILURE";
    case -12: return "CL_MAP_FAILURE";
    case -13: return "CL_MISALIGNED_SUB_BUFFER_OFFSET";
    case -14: return "CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST";
    case -15: return "CL_COMPILE_PROGRAM_FAILURE";
    case -16: return "CL_LINKER_NOT_AVAILABLE";
    case -17: return "CL_LINK_PROGRAM_FAILURE";
    case -18: return "CL_DEVICE_PARTITION_FAILED";
    case -19: return "CL_KERNEL_ARG_INFO_NOT_AVAILABLE";

    // compile-time errors
    case -30: return "CL_INVALID_VALUE";
    case -31: return "CL_INVALID_DEVICE_TYPE";
    case -32: return "CL_INVALID_PLATFORM";
    case -33: return "CL_INVALID_DEVICE";
    case -34: return "CL_INVALID_CONTEXT";
    case -35: return "CL_INVALID_QUEUE_PROPERTIES";
    case -36: return "CL_INVALID_COMMAND_QUEUE";
    case -37: return "CL_INVALID_HOST_PTR";
    case -38: return "CL_INVALID_MEM_OBJECT";
    case -39: return "CL_INVALID_IMAGE_FORMAT_DESCRIPTOR";
    case -40: return "CL_INVALID_IMAGE_SIZE";
    case -41: return "CL_INVALID_SAMPLER";
    case -42: return "CL_INVALID_BINARY";
    case -43: return "CL_INVALID_BUILD_OPTIONS";
    case -44: return "CL_INVALID_PROGRAM";
    case -45: return "CL_INVALID_PROGRAM_EXECUTABLE";
    case -46: return "CL_INVALID_KERNEL_NAME";
    case -47: return "CL_INVALID_KERNEL_DEFINITION";
    case -48: return "CL_INVALID_KERNEL";
    case -49: return "CL_INVALID_ARG_INDEX";
    case -50: return "CL_INVALID_ARG_VALUE";
    case -51: return "CL_INVALID_ARG_SIZE";
    case -52: return "CL_INVALID_KERNEL_ARGS";
    case -53: return "CL_INVALID_WORK_DIMENSION";
    case -54: return "CL_INVALID_WORK_GROUP_SIZE";
    case -55: return "CL_INVALID_WORK_ITEM_SIZE";
    case -56: return "CL_INVALID_GLOBAL_OFFSET";
    case -57: return "CL_INVALID_EVENT_WAIT_LIST";
    case -58: return "CL_INVALID_EVENT";
    case -59: return "CL_INVALID_OPERATION";
    case -60: return "CL_INVALID_GL_OBJECT";
    case -61: return "CL_INVALID_BUFFER_SIZE";
    case -62: return "CL_INVALID_MIP_LEVEL";
    case -63: return "CL_INVALID_GLOBAL_WORK_SIZE";
    case -64: return "CL_INVALID_PROPERTY";
    case -65: return "CL_INVALID_IMAGE_DESCRIPTOR";
    case -66: return "CL_INVALID_COMPILER_OPTIONS";
    case -67: return "CL_INVALID_LINKER_OPTIONS";
    case -68: return "CL_INVALID_DEVICE_PARTITION_COUNT";
    case -69:  return "CL_INVALID_PIPE_SIZE";
    case -70: return "CL_INVALID_DEVICE_QUEUE";

    default: return "Unknown OpenCL error";
 }
}

static void check_error(cl_int error, char const *name) {
    if (error != CL_SUCCESS) {
        fprintf(stderr, "Non-successful return code %d (%s) for %s.  Exiting.\n", error, getErrorString(error), name);
        exit(EXIT_FAILURE);
    }
}

static void exit_msg(char const *str){
        fprintf(stderr, "%s \n", str);
        exit(EXIT_FAILURE);
}

// =================================================================================================

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
            err = clGetDeviceInfo(device, CL_DEVICE_NAME, 128, name, NULL);
            check_error(err, "clGetPlatformInfo");
            printf("-- Device #%d: %s\n", device_idx, name);
         }
    }
}
// =================================================================================================
