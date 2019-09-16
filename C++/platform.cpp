// Includes
#define CL_HPP_TARGET_OPENCL_VERSION 200
#include <CL/cl2.hpp>
#include <iostream>
#include <vector>

int main(int argc, char* argv[]) {

    /* - - - -
    Plateform
    - - - - */
    //A platform is a specific OpenCL implementation, for instance AMD, NVIDIA or Intel.
    // Intel may have a different OpenCL implementation for the CPU and GPU.

    // Discover platforms:
    std::vector<cl::Platform> platforms;
    cl::Platform::get(&platforms);
    for (auto &plat : platforms) {
        std::cout << "Platform: " << plat.getInfo<CL_PLATFORM_NAME>() << std::endl;
    }

}
