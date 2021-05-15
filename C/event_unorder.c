// Includes
#include <stdio.h>
#include <CL/opencl.h>
#include <unistd.h>
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
    // A context is a platform with a set of available devices for that platform.
    cl_context context = clCreateContext(0, device_count, devices, NULL, NULL, &err);
    check_error(err,"clCreateContext");

    /* - - - -
    Command queue
    - - - - */
    // The OpenCL functions that are submitted to a command-queue are enqueued in the order the calls are made but can be configured to execute in-order or out-of-order.
    cl_command_queue queue = clCreateCommandQueue(context, device, CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE, &err);
    check_error(err,"clCreateCommandQueue");

    // |/  _  ._ ._   _  |
    // |\ (/_ |  | | (/_ |
    //
    printf(">>> Kernel configuration...\n");

    // Hardcoded
    const char *kernelstring =
        "__kernel void hello_world(int id) {"
        "   printf(\"Hello world from id %d\\n\", id);"
        "}";

    // Create the program
    cl_program program = clCreateProgramWithSource(context, 1, &kernelstring, NULL, &err);
    check_error(err,"clCreateProgramWithSource");

    //Build / Compile the program executable
    err = clBuildProgram(program, device_count, devices, "", NULL, NULL);
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

   /* - - - -
    Create
    - - - - */
    cl_kernel kernel = clCreateKernel(program, "hello_world", &err);
    check_error(err,"clCreateKernel");

    /* - - - -
    ND range
    - - - - */
    printf(">>> NDrange configuration...\n");

    #define WORK_DIM 1

    // Describe the number of global work-items in work_dim dimensions that will execute the kernel function
    size_t global0 = (size_t) atoi(argv[3]);
    const size_t global[WORK_DIM] = {  global0 };

    // Describe the number of work-items that make up a work-group (also referred to as the size of the work-group).
    // local_work_size can also be a NULL value in which case the OpenCL implementation will determine how to be break the global work-items into appropriate work-group instances.
    size_t local0 = (size_t) atoi(argv[4]);
    const size_t local[WORK_DIM] = { local0 };

    printf("Global work size: %zu \n", global[0]);
    printf("Local work size: %zu \n", local[0]);

    /* - - - -
    Execute
    - - - - */
    printf(">>> Kernel Execution...\n");

    const size_t num_kernel = (size_t) atoi(argv[5]) ;
    cl_event ev;
    ev = clCreateUserEvent(context, NULL);
    for (size_t id = 0 ; id < num_kernel; id++){
         err = clSetKernelArg(kernel, 0, sizeof(id), &id);
         check_error(err,"clSetKernelArg");

         err  = clEnqueueNDRangeKernel(queue, kernel, WORK_DIM, NULL, global, local, id%2 == 0 ? 0 : 1, id%2 == 0 ? NULL : &ev, NULL);
         check_error(err,"clEnqueueNDRangeKernel");
    }
    sleep(1);
    clSetUserEventStatus(ev, CL_COMPLETE);


    /* - - -
    Sync & check
    - - - */
    clFinish(queue);

    //  _                         
    // /  |  _   _. ._  o ._   _  
    // \_ | (/_ (_| | | | | | (_| 
    //                         _| 
    clReleaseCommandQueue(queue);
    clReleaseContext(context);
    clReleaseProgram(program);
    clReleaseKernel(kernel);

    // Exit
    return 0;
}

// =================================================================================================
