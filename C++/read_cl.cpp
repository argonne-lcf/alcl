#define CL_HPP_TARGET_OPENCL_VERSION 200
#include <CL/cl2.hpp>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cstdlib>
#include "./clerror.h"

int main(int argc, char* argv[]) {


    if (argc < 5) {
        std::cerr << argv[0] << " platform_id device_id global_size local_size" << std::endl;
        exit(1);
    }

    int platform_idx = atoi(argv[1]);
    int device_idx = atoi(argv[2]);
    int global_size = atoi(argv[3]);
    int local_size = atoi(argv[4]);

    try {
        std::vector<cl::Platform> platforms;
        cl::Platform::get(&platforms);

        cl::Platform plat = platforms[platform_idx];
        std::cout << "Platform: " << plat.getInfo<CL_PLATFORM_NAME>() << std::endl;

        std::vector<cl::Device> devices;
        plat.getDevices(CL_DEVICE_TYPE_ALL, &devices);
        std::cout << "  -- Device: " << devices[device_idx].getInfo<CL_DEVICE_NAME>() << std::endl;

        cl::Context context(devices);

        cl::CommandQueue queue(context, devices[device_idx]);

        std::ifstream file("hwv.cl");
        std::ostringstream kernelstring;
        kernelstring << file.rdbuf();

        cl::Program program(context, kernelstring.str());
        program.build("-cl-unsafe-math-optimizations");

        cl::Kernel kernel(program, "hello_world");

        auto kernel_func = cl::KernelFunctor<>(kernel);

        kernel_func(cl::EnqueueArgs(queue, cl::NDRange(global_size), cl::NDRange(local_size)));

        queue.finish();
    } catch (cl::Error e) {
        std::cout << e.what()<< "(" << e.err() << ": " << getErrorString(e.err()) << ")" << std::endl;
        exit(1);
    }
}
