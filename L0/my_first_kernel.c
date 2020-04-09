#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <level_zero/ze_api.h>

#include "./ze_utils.h"

int main(int argc, char* argv[]) {
  ze_result_t errno;

  if (argc < 3)
       exit_msg(strcat(argv[0], " groupSize numGroup" ));

  //  _              _                      _
  // |_) |  _. _|_ _|_ _  ._ ._ _    ()    | \  _     o  _  _
  // |   | (_|  |_  | (_) |  | | |   (_X   |_/ (/_ \/ | (_ (/_
  //
  printf(">>> Initializing OpenCL Platform and Device...\n");
  // Select the first GPU avalaible

  // Initialize the driver
  errno  = zeInit(ZE_INIT_FLAG_NONE);
  check_error(errno, "zeInit");

  // Discover all the driver instances
  uint32_t driverCount = 0;
  errno = zeDriverGet(&driverCount, NULL);
  check_error(errno, "zeDriverGet");

  //Now where the phDrivers
  ze_driver_handle_t* phDrivers = (ze_driver_handle_t*) malloc(driverCount * sizeof(ze_driver_handle_t));
  errno = zeDriverGet(&driverCount, phDrivers);
  check_error(errno, "zeDriverGet");

  // Device who will be selected
  ze_device_handle_t hDevice = NULL;

  for(uint32_t driver_idx = 0; driver_idx < driverCount; driver_idx++) {

    ze_driver_handle_t driver = phDrivers[driver_idx];
    /* - - - -
    Device
    - - - - */

    // if count is zero, then the driver will update the value with the total number of devices available.
    uint32_t deviceCount = 0;
    errno = zeDeviceGet(driver, &deviceCount, NULL);
    check_error(errno, "zeDeviceGet");

    ze_device_handle_t* phDevices = (ze_device_handle_t*) malloc(deviceCount * sizeof(ze_device_handle_t));
    errno = zeDeviceGet(driver, &deviceCount, phDevices);
    check_error(errno, "zeDeviceGet");

    for(uint32_t device_idx = 0;  device_idx < deviceCount; device_idx++) {
        ze_device_properties_t device_properties;
        errno = zeDeviceGetProperties(phDevices[device_idx], &device_properties);
        check_error(errno, "zeDeviceGetProperties");
        if (device_properties.type == ZE_DEVICE_TYPE_GPU){
                printf("Running on Device #%d %s who is a GPU. \n", device_idx, device_properties.name);
                hDevice = phDevices[device_idx];
                break;
        }
    }

    free(phDevices);
    if (hDevice != NULL) {
        break;
    }
  }

  free(phDrivers);

 //    _                                _                  
 //  /   _  ._ _  ._ _   _. ._   _|   / \      _       _  
 //  \_ (_) | | | | | | (_| | | (_|   \_X |_| (/_ |_| (/_ 
 //                                                       

  // Command queue
  ze_command_queue_desc_t commandQueueDesc = {
    ZE_COMMAND_QUEUE_DESC_VERSION_CURRENT,
    ZE_COMMAND_QUEUE_FLAG_NONE,
    ZE_COMMAND_QUEUE_MODE_DEFAULT,
    ZE_COMMAND_QUEUE_PRIORITY_NORMAL,
    0
  };
  ze_command_queue_handle_t hCommandQueue;
  errno = zeCommandQueueCreate(hDevice, &commandQueueDesc, &hCommandQueue);
  check_error(errno, "zeCommandQueueCreate");

 //   _                                           
 //  /   _  ._ _  ._ _   _. ._   _|   |  o  _ _|_ 
 //  \_ (_) | | | | | | (_| | | (_|   |_ | _>  |_ 
 //                                               

  ze_command_list_desc_t commandListDesc = {
    ZE_COMMAND_LIST_DESC_VERSION_CURRENT,
    ZE_COMMAND_LIST_FLAG_NONE
  };
  ze_command_list_handle_t hCommandList;
  errno =  zeCommandListCreate(hDevice, &commandListDesc, &hCommandList);
  check_error(errno, "zeCommandListCreate");

  //                        
  // |\/|  _   _|     |  _  
  // |  | (_) (_| |_| | (/_ 
  //                        

  unsigned char* program_file; size_t program_size;
  errno = read_from_binary(&program_file, &program_size, "hwv.ar");
  check_error(errno, "read_from_binary");

   // OpenCL C kernel has been compiled to Gen Binary
  ze_module_desc_t moduleDesc = {
    ZE_MODULE_DESC_VERSION_CURRENT,
    ZE_MODULE_FORMAT_NATIVE,
    program_size,
    program_file,
    NULL,
    NULL
  };
  ze_module_handle_t hModule;
  errno = zeModuleCreate(hDevice, &moduleDesc, &hModule, NULL);
  check_error(errno, "zeModuleCreate");
                     
  //   |/  _  ._ ._   _  | 
  //   |\ (/_ |  | | (/_ | 
  //                       

  ze_kernel_desc_t kernelDesc = {
    ZE_KERNEL_DESC_VERSION_CURRENT,
    ZE_KERNEL_FLAG_NONE,
    "hello_world"
  };
  ze_kernel_handle_t hKernel;
  errno = zeKernelCreate(hModule, &kernelDesc, &hKernel);
  check_error(errno, "zeKernelCreate");
 
  uint32_t groupSizeX =  (uint32_t) atoi(argv[1]);
  uint32_t numGroupsX =  (uint32_t) atoi(argv[2]);

  zeKernelSetGroupSize(hKernel, groupSizeX, 1, 1);

  //  __                                 
  // (_      |_  ._ _  o  _  _ o  _  ._  
  // __) |_| |_) | | | | _> _> | (_) | | 
  //                                     
  
  ze_group_count_t launchArgs = { numGroupsX, 1, 1 };
  // Append launch kernel
  zeCommandListAppendLaunchKernel(hCommandList, hKernel, &launchArgs, NULL, 0, NULL);

  // finished appending commands (typically done on another thread)
  errno = zeCommandListClose(hCommandList);
  check_error(errno, "zeCommandListClose");

  // Execute command list in command queue
  errno = zeCommandQueueExecuteCommandLists(hCommandQueue, 1, &hCommandList, NULL);
  check_error(errno, "zeCommandQueueExecuteCommandLists");

  // synchronize host and device
  errno = zeCommandQueueSynchronize(hCommandQueue, UINT32_MAX);
  check_error(errno, "zeCommandQueueSynchronize");

  //   _                         
  //  /  |  _   _. ._  o ._   _  
  //  \_ | (/_ (_| | | | | | (_| 
  //                          _|
  
  errno = zeKernelDestroy(hKernel);
  check_error(errno, "zeKernelDestroy");

  errno = zeModuleDestroy(hModule);
  check_error(errno, "zeModuleDestroy");
 
  errno =  zeCommandListDestroy(hCommandList);
  check_error(errno, "zeCommandListDestroy");
  /*
  // If using command list again
  // Reset (recycle) command list for new commands
  errno = zeCommandListReset(hCommandList);
  check_error(errno, "zeCommandListReset");
  */

  errno = zeCommandQueueDestroy(hCommandQueue);
  check_error(errno, "zeCommandQueueDestroy");
  return 0;
}
