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
  errno  = zeInit(ZE_INIT_FLAG_GPU_ONLY);
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
  ze_driver_handle_t hDriver = NULL;
  ze_device_properties_t device_properties;

  for(uint32_t driver_idx = 0; driver_idx < driverCount; driver_idx++) {

    hDriver = phDrivers[driver_idx];
    /* - - - -
    Device
    - - - - */

    // if count is zero, then the driver will update the value with the total number of devices available.
    uint32_t deviceCount = 0;
    errno = zeDeviceGet(hDriver, &deviceCount, NULL);
    check_error(errno, "zeDeviceGet");

    ze_device_handle_t* phDevices = (ze_device_handle_t*) malloc(deviceCount * sizeof(ze_device_handle_t));
    errno = zeDeviceGet(hDriver, &deviceCount, phDevices);
    check_error(errno, "zeDeviceGet");

    for(uint32_t device_idx = 0;  device_idx < deviceCount; device_idx++) {
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
  //    _
  //   /   _  ._ _|_  _    _|_
  //   \_ (_) | | |_ (/_ >< |_
  //
  ze_context_handle_t hContext = NULL;
  // Create context
  ze_context_desc_t context_desc = {
     ZE_STRUCTURE_TYPE_CONTEXT_DESC,
     NULL,
     0};
  errno = zeContextCreate(hDriver, &context_desc, &hContext);
  check_error(errno, "zeContextCreate");

  //    _                                _
  //  /   _  ._ _  ._ _   _. ._   _|   / \      _       _
  //  \_ (_) | | | | | | (_| | | (_|   \_X |_| (/_ |_| (/_
  //
   // Discover all command queue types
   uint32_t cmdqueueGroupCount = 0;
   zeDeviceGetCommandQueueGroupProperties(hDevice, &cmdqueueGroupCount, NULL);

   ze_command_queue_group_properties_t* cmdqueueGroupProperties = (ze_command_queue_group_properties_t*) malloc(cmdqueueGroupCount * sizeof(ze_command_queue_group_properties_t));
   errno = zeDeviceGetCommandQueueGroupProperties(hDevice, &cmdqueueGroupCount, cmdqueueGroupProperties);
   check_error(errno, "zeDeviceGetCommandQueueGroupProperties");

   // Find a proper command queue
   uint32_t computeQueueGroupOrdinal = cmdqueueGroupCount;
   for( uint32_t i = 0; i < cmdqueueGroupCount; ++i ) {
      if( cmdqueueGroupProperties[ i ].flags & ZE_COMMAND_QUEUE_GROUP_PROPERTY_FLAG_COMPUTE ) {
        computeQueueGroupOrdinal = i;
        break;
    }
  }
  // Should put some error handling is not found

  // Command queue
  ze_command_queue_desc_t commandQueueDesc = {
    ZE_STRUCTURE_TYPE_COMMAND_QUEUE_DESC,
    NULL,
    computeQueueGroupOrdinal,
    0, // index
    0, // flags
    ZE_COMMAND_QUEUE_MODE_DEFAULT,
    ZE_COMMAND_QUEUE_PRIORITY_NORMAL
  };

  ze_command_queue_handle_t hCommandQueue;
  errno = zeCommandQueueCreate(hContext, hDevice, &commandQueueDesc, &hCommandQueue);
  check_error(errno, "zeCommandQueueCreate");
  //   _
  //  /   _  ._ _  ._ _   _. ._   _|   |  o  _ _|_
  //  \_ (_) | | | | | | (_| | | (_|   |_ | _>  |_
  //
  ze_command_list_desc_t commandListDesc = {
    ZE_STRUCTURE_TYPE_COMMAND_LIST_DESC,
    NULL,
    computeQueueGroupOrdinal,
    0 // flags
  };
  ze_command_list_handle_t hCommandList;
  errno =  zeCommandListCreate(hContext, hDevice, &commandListDesc, &hCommandList);
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
    ZE_STRUCTURE_TYPE_MODULE_DESC,
    NULL,
    ZE_MODULE_FORMAT_NATIVE,
    program_size,
    program_file,
    NULL,
    NULL
  };
  ze_module_handle_t hModule;
  errno = zeModuleCreate(hContext, hDevice, &moduleDesc, &hModule, NULL);
  check_error(errno, "zeModuleCreate");

  //   |/  _  ._ ._   _  |
  //   |\ (/_ |  | | (/_ |
  //

  ze_kernel_desc_t kernelDesc = {
    ZE_STRUCTURE_TYPE_KERNEL_DESC,
    NULL,
    0, // flags
    "hello_world"
  };
  ze_kernel_handle_t hKernel;
  errno = zeKernelCreate(hModule, &kernelDesc, &hKernel);
  check_error(errno, "zeKernelCreate");

  uint32_t groupSizeX =  (uint32_t) atoi(argv[1]);
  uint32_t numGroupsX =  (uint32_t) atoi(argv[2]);

  zeKernelSetGroupSize(hKernel, groupSizeX, 1, 1);
  
  //  _               
  // |_     _  ._ _|_ 
  // |_ \/ (/_ | | |_ 
  //                  

  // Create event pool
  ze_event_pool_desc_t eventPoolDesc = {
    ZE_STRUCTURE_TYPE_EVENT_POOL_DESC,
    NULL,
    //ZE_EVENT_POOL_FLAG_HOST_VISIBLE | ZE_EVENT_POOL_FLAG_KERNEL_TIMESTAMP,
    ZE_EVENT_POOL_FLAG_KERNEL_TIMESTAMP,
    1 // One event on the pool
  };

  ze_event_pool_handle_t hEventPool;
  errno = zeEventPoolCreate(hContext, &eventPoolDesc, 0, NULL, &hEventPool);
  //errno = zeEventPoolCreate(hDriver, &eventPoolDesc, 1, &hDevice, &hEventPool);
  check_error(errno, "zeEventPoolCreate");

  ze_event_desc_t eventDesc = {
    ZE_STRUCTURE_TYPE_EVENT_DESC,
    NULL,
    0, // index
    0, // no memory/cache coherency required on signal
    //ZE_EVENT_SCOPE_FLAG_HOST  // ensure memory coherency across device and Host after event completes
    0 // No need for memory/cache coherency on wait
  };
  ze_event_handle_t hEvent;
  errno = zeEventCreate(hEventPool, &eventDesc, &hEvent);
  check_error(errno, "zeEventCreate");

  // allocate memory for results
  ze_device_mem_alloc_desc_t tsResultDesc = {
    ZE_STRUCTURE_TYPE_DEVICE_MEM_ALLOC_DESC,
    NULL,
    0, // flags
    0  // ordinal
  };

  ze_kernel_timestamp_result_t* tsResult = NULL;
  errno = zeMemAllocDevice(hContext, &tsResultDesc, sizeof(ze_kernel_timestamp_result_t), sizeof(uint32_t), hDevice, (void**) &tsResult);
  check_error(errno, "zeMemAllocDevice");

  //  __                                 
  // (_      |_  ._ _  o  _  _ o  _  ._  
  // __) |_| |_) | | | | _> _> | (_) | | 
  //                                     
  
  ze_group_count_t launchArgs = { numGroupsX, 1, 1 };
  // Append launch kernel
  errno = zeCommandListAppendLaunchKernel(hCommandList, hKernel, &launchArgs, hEvent, 0, NULL);
  check_error(errno, "zeCommandListAppendLaunchKernel");

  // Append a query of a timestamp event into the command list
  errno = zeCommandListAppendQueryKernelTimestamps(hCommandList, 1, &hEvent, tsResult, NULL, NULL, 1, &hEvent);

  // finished appending commands (typically done on another thread)
  errno = zeCommandListClose(hCommandList);
  check_error(errno, "zeCommandListClose");

  // Execute command list in command queue
  errno = zeCommandQueueExecuteCommandLists(hCommandQueue, 1, &hCommandList, NULL);
  check_error(errno, "zeCommandQueueExecuteCommandLists");

  // synchronize host and device
  errno = zeCommandQueueSynchronize(hCommandQueue, UINT32_MAX);
  check_error(errno, "zeCommandQueueSynchronize");

  // Why not?
  errno = zeEventHostSynchronize(hEvent, UINT32_MAX);
  check_error(errno, "zeEventHostSynchronize");

  //    _                                              
  //   |_     _   _    _|_ o  _  ._    _|_ o ._ _   _  
  //   |_ >< (/_ (_ |_| |_ | (_) | |    |_ | | | | (/_ 
  //                                                   

  // Get timestamp frequency
  const uint64_t timestampFreq = device_properties.timerResolution;
  const uint64_t timestampMaxValue = ~(-1 << device_properties.kernelTimestampValidBits);

  double globalTimeInNs = ( tsResult->global.kernelEnd >= tsResult->global.kernelStart )
    ? ( tsResult->global.kernelEnd - tsResult->global.kernelStart ) * (double)timestampFreq
    : (( timestampMaxValue - tsResult->global.kernelStart) + tsResult->global.kernelEnd + 1 ) * (double)timestampFreq;
  printf("Time on GPU: %f ns\n", globalTimeInNs);

  //   _                         
  //  /  |  _   _. ._  o ._   _  
  //  \_ | (/_ (_| | | | | | (_| 
  //                          _|
  errno = zeEventDestroy(hEvent);
  check_error(errno, "zeEventDestroy");
  
  // If not calling 'zeEventDestroy' got an 'ZE_RESULT_ERROR_INVALID_ARGUMENT'.  
  errno = zeEventPoolDestroy(hEventPool);
  check_error(errno, "zeEventPoolDestroy");

  errno = zeKernelDestroy(hKernel);
  check_error(errno, "zeKernelDestroy");

  errno = zeModuleDestroy(hModule);
  check_error(errno, "zeModuleDestroy");
 
  errno =  zeCommandListDestroy(hCommandList);
  check_error(errno, "zeCommandListDestroy");

  errno = zeCommandQueueDestroy(hCommandQueue);
  check_error(errno, "zeCommandQueueDestroy");
  return 0;
}
