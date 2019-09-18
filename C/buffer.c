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
    // A context is a platform with a set of available devices for that platform.
    cl_context context = clCreateContext(0, device_count, devices, NULL, NULL, &err);
    check_error(err,"clCreateContext");

    /* - - - -
    Command queue
    - - - - */
    // The OpenCL functions that are submitted to a command-queue are enqueued in the order the calls are made but can be configured to execute in-order or out-of-order.
    const cl_queue_properties properties[] =  { CL_QUEUE_PROPERTIES, (CL_QUEUE_PROFILING_ENABLE), 0 };

    cl_command_queue queue = clCreateCommandQueueWithProperties(context, device, properties, &err);
    check_error(err,"clCreateCommandQueueWithProperties");

    //  _       _   _
    // |_)    _|_ _|_ _  ._
    // |_) |_| |   | (/_ |

    // Length of vectors
    size_t n =  {   (size_t) atoi(argv[3]) };
    size_t bytes = n*sizeof(double);
    double *h_a = (double*)malloc(bytes);
    cl_mem d_a = clCreateBuffer(context, CL_MEM_WRITE_ONLY | CL_MEM_HOST_READ_ONLY, bytes, NULL, &err);
    check_error(err,"cclCreateBuffer");

    // |/  _  ._ ._   _  |
    // |\ (/_ |  | | (/_ |
    //
    printf(">>> Kernel configuration...\n");

    // String kernel.
    // Use MPI terminology (we are HPC!)
    const char *kernelstring =
        "__kernel void hello_world(__global double *a, const unsigned int n) {"
        "   const int world_rank = get_global_id(0);"
        "    if (world_rank < n)"
        "    a[world_rank] =  world_rank;"
        "}";

    // Create the program
    cl_program program = clCreateProgramWithSource(context, 1, &kernelstring, NULL, &err);
    check_error(err,"clCreateProgramWithSource");

    //Build / Compile the program executable
    err = clBuildProgram(program, device_count, devices, "-cl-unsafe-math-optimizations", NULL, NULL);
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

    err  = clSetKernelArg(kernel, 0, sizeof(cl_mem), &d_a);
    err |= clSetKernelArg(kernel, 1, sizeof(unsigned int), &n);
    check_error(err,"clSetKernelArg");

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

    err  = clEnqueueNDRangeKernel(queue, kernel, WORK_DIM, NULL, global, local, 0, NULL, NULL);
    check_error(err,"clEnqueueNDRangeKernel");

    clEnqueueReadBuffer(queue, d_a, CL_FALSE, 0, bytes, h_a, 0, NULL, NULL );
    /* - - - 
    Sync & check
    - - - */

    // Wait for the command queue to get serviced before reading back results
    clFinish(queue);

    // Read the results from the device
    double sum = 0;
    for(int i=0; i<n; i++)
        sum += h_a[i];
     printf("final result: %f, should have been %lu\n", sum, n*(n-1)/2);

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
