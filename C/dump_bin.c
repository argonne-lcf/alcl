// Includes
#include <stdio.h>
#include <CL/cl.h>
#include "./cl_utils.h"

int main(int argc, char* argv[]) {

   cl_int err;
    
   if (argc < 5)
        exit_msg("Not enought arguments.");

   //  _              _                      _
   // |_) |  _. _|_ _|_ _  ._ ._ _    ()    | \  _     o  _  _
   // |   | (_|  |_  | (_) |  | | |   (_X   |_/ (/_ \/ | (_ (/_
   //
    printf(">>> Initializing OpenCL Platform and Device...\n");

    cl_uint platform_idx = (cl_uint) atoi(argv[1]);
    cl_uint device_idx = (cl_uint) atoi(argv[2]);
    
    char name[128];
    /* - - - 
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

    cl_device_id device = devices[device_idx];
    err = clGetDeviceInfo(device, CL_DEVICE_NAME, 128, name, NULL);
    check_error(err, "clGetPlatformInfo");

    printf("-- Device #%d: %s\n", device_idx, name);
    
    //  _                               _                  
    // /   _  ._ _|_  _    _|_   ()    / \      _       _  
    // \_ (_) | | |_ (/_ >< |_   (_X   \_X |_| (/_ |_| (/_ 
    //                                                     

    /* - - - -
    Context
    - - - - */
    // A context is a platform with the (or several) selected device for that platform.
    cl_context context = clCreateContext(0, 1, &device, NULL, NULL, &err);
    check_error(err,"clCreateContext");

    // Readed from file
    char* kernelstring = read_from_file(argv[3]);

    // Create the program
    cl_program program = clCreateProgramWithSource(context, 1, (const char **) &kernelstring, NULL, &err);
    check_error(err,"clCreateProgramWithSource");

    //Build / Compile the program executable
    err = clBuildProgram(program, 1, &device, "-cl-unsafe-math-optimizations", NULL, NULL);
    if (err != CL_SUCCESS)
    {
        printf("Error: Failed to build program executable!\n");

        size_t logSize;
        clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, 0, NULL, &logSize);

        // there's no information in the reference whether the string is 0 terminated or not.
        char* messages = (char*)malloc((1+logSize)*sizeof(char));
        clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, logSize, messages, NULL);
        messages[logSize] = '\0';

        printf("%s", messages);
        free(messages);
        return EXIT_FAILURE;
    }

    size_t program_size;

    err = clGetProgramInfo(program, CL_PROGRAM_BINARY_SIZES, sizeof(size_t), &program_size, NULL);
    check_error(err, "clGetProgramInfo(CL_PROGRAM_BINARY_SIZES)");

    unsigned char *bin = (unsigned char*)malloc(program_size);

    err = clGetProgramInfo(program, CL_PROGRAM_BINARIES, program_size, &bin, NULL);
    check_error(err, "clGetProgramInfo(CL_PROGRAM_BINARIES)");

    write_to_binary(bin, program_size, argv[4]);
    free(bin);
    free(kernelstring);

    //  _                         
    // /  |  _   _. ._  o ._   _  
    // \_ | (/_ (_| | | | | | (_| 
    //                         _| 
    clReleaseContext(context);
    clReleaseProgram(program);

    // Exit
    return 0;
}

// =================================================================================================
