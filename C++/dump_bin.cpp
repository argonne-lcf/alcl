#define CL_HPP_TARGET_OPENCL_VERSION 200
#define CL_HPP_ENABLE_EXCEPTIONS 1
#include <CL/cl2.hpp>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cstdlib>
#include "./clerror.h"

int main(int argc, char* argv[]) {


    if (argc < 5) {
        std::cerr << argv[0] << " platform_id device_id input output" << std::endl;
        exit(1);
    }

    int platform_idx = atoi(argv[1]);
    int device_idx = atoi(argv[2]);

    try {
        std::vector<cl::Platform> platforms;
        cl::Platform::get(&platforms);

        cl::Platform plat = platforms[platform_idx];
        std::cout << "Platform: " << plat.getInfo<CL_PLATFORM_NAME>() << std::endl;

        std::vector<cl::Device> devices;
        plat.getDevices(CL_DEVICE_TYPE_ALL, &devices);
        std::cout << "  -- Device: " << devices[device_idx].getInfo<CL_DEVICE_NAME>() << std::endl;

        cl::Context context(devices[device_idx]);

        std::ifstream file(argv[3]);
        std::ostringstream kernelstring;
        kernelstring << file.rdbuf();

        cl::Program program(context, kernelstring.str());
        program.build("-cl-unsafe-math-optimizations");

        std::ofstream ofile(argv[4], std::ofstream::binary);
        auto bin = program.getInfo<CL_PROGRAM_BINARIES>()[0];
        ofile.write((char*)&bin[0], bin.size());

    } catch (cl::Error e) {
        std::cout << e.what()<< "(" << e.err() << ": " << getErrorString(e.err()) << ")" << std::endl;
        exit(1);
    }
}
