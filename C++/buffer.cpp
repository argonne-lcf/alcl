#define CL_HPP_TARGET_OPENCL_VERSION 200
#define CL_HPP_ENABLE_EXCEPTIONS 1
#include <CL/cl2.hpp>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <numeric>
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

        std::string kernelstring{R"CLC(
        __kernel void hello_world(__global double *a, const unsigned int n) {
            const int world_rank = get_global_id(0);
            if (world_rank < n)
            a[world_rank] =  world_rank;
        }
        )CLC"};

        cl::Program program(context, kernelstring);
        program.build("-cl-unsafe-math-optimizations");

        cl::Kernel kernel(program, "hello_world");

        auto kernel_func = cl::KernelFunctor<cl::Buffer, int>(kernel);

        std::vector<double> h_a(global_size);
        cl::Buffer d_a(context, CL_MEM_WRITE_ONLY | CL_MEM_HOST_READ_ONLY, global_size*sizeof(double));

        kernel_func(cl::EnqueueArgs(queue, cl::NDRange(global_size), cl::NDRange(local_size)), d_a, global_size);
        queue.enqueueReadBuffer(d_a, false, 0, global_size*sizeof(double), h_a.data());
        queue.finish();

        double sum = std::accumulate(h_a.begin(), h_a.end(), 0.0);
        std::cout << "Final result: " << sum << ", should have been " << global_size*(global_size-1)/2 << std::endl;

    } catch (cl::Error e) {
        std::cout << e.what()<< "(" << e.err() << ": " << getErrorString(e.err()) << ")" << std::endl;
        exit(1);
    }
    return 0;
}

// =================================================================================================
