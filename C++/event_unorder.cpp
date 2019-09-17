#define CL_HPP_TARGET_OPENCL_VERSION 200
#define CL_HPP_ENABLE_EXCEPTIONS 1
#include <CL/cl2.hpp>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cstdlib>
#include "./clerror.h"
#include <unistd.h>

int main(int argc, char* argv[]) {


    if (argc < 6) {
        std::cerr << argv[0] << " platform_id device_id global_size local_size num_kernel" << std::endl;
        exit(1);
    }

    int platform_idx = atoi(argv[1]);
    int device_idx = atoi(argv[2]);
    int global_size = atoi(argv[3]);
    int local_size = atoi(argv[4]);
    int num_kernel = atoi(argv[5]);

    try {
        std::vector<cl::Platform> platforms;
        cl::Platform::get(&platforms);

        cl::Platform plat = platforms[platform_idx];
        std::cout << "Platform: " << plat.getInfo<CL_PLATFORM_NAME>() << std::endl;

        std::vector<cl::Device> devices;
        plat.getDevices(CL_DEVICE_TYPE_ALL, &devices);
        std::cout << "  -- Device: " << devices[device_idx].getInfo<CL_DEVICE_NAME>() << std::endl;

        cl::Context context(devices);

        cl::CommandQueue queue(context, devices[device_idx], CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE);

        std::string kernelstring{R"CLC(
            __kernel void hello_world(const int id) {
                printf("Hello world from id %d \n", id);
            }
        )CLC"};

        cl::Program program(context, kernelstring);
        program.build("-cl-unsafe-math-optimizations");

        cl::Kernel kernel(program, "hello_world");
        auto kernel_func = cl::KernelFunctor<int>(kernel);

        cl::UserEvent event(context);

        for(int id = 0; id < num_kernel; id++) {
            if (id % 2 == 0) {
                kernel_func(cl::EnqueueArgs(queue, cl::NDRange(global_size), cl::NDRange(local_size)), id);
            } else {
                kernel_func(cl::EnqueueArgs(queue, event, cl::NDRange(global_size), cl::NDRange(local_size)), id);
            }
        }

        sleep(1);
        event.setStatus(CL_COMPLETE);

        queue.finish();
    } catch (cl::Error e) {
        std::cout << e.what()<< "(" << e.err() << ": " << getErrorString(e.err()) << ")" << std::endl;
        exit(1);
    }
}
