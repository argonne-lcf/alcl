#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <level_zero/ze_api.h>

#include "./ze_utils.h"

#include <pthread.h>

#define NUM_THREADS  10

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

struct s_thread_data{
   ze_command_list_handle_t hCommandList;
   ze_kernel_handle_t hKernel;
   ze_group_count_t launchArgs;
};

//struct thread_data thread_data_array[NUM_THREADS];

void *myThreadFun(void *threadarg) 
{ 
    struct s_thread_data mydata = * (struct s_thread_data *) threadarg;

    pthread_mutex_lock(&mutex);
    zeCommandListAppendLaunchKernel(mydata.hCommandList, mydata.hKernel, &mydata.launchArgs, NULL, 0, NULL);
    pthread_mutex_unlock(&mutex);

    pthread_exit(NULL);
} 

int main(int argc, char* argv[]) {
  ze_result_t errno;

  if (argc < 3)
       exit_msg(strcat(argv[0], " groupSize numGroup" ));

  //  _              _                      _
  // |_) |  _. _|_ _|_ _  ._ ._ _    ()    | \  _     o  _  _
  // |   | (_|  |_  | (_) |  | | |   (_X   |_/ (/_ \/ | (_ (/_
  //
  printf(">>> Initializing L0 Platform and Device...\n");
  // Select the first GPU avalaible

  // Initialize the driver and discover all the driver instances
  ze_init_driver_type_desc_t driverTypeDesc = { .stype = ZE_STRUCTURE_TYPE_INIT_DRIVER_TYPE_DESC,
    .flags = ZE_INIT_DRIVER_TYPE_FLAG_GPU };
  uint32_t driverCount = 0;
  errno = zeInitDrivers(&driverCount, NULL, &driverTypeDesc);
  check_error(errno, "zeInitDrivers");

  //Now where the phDrivers
  ze_driver_handle_t* phDrivers = (ze_driver_handle_t*) malloc(driverCount * sizeof(ze_driver_handle_t));
  errno = zeInitDrivers( &driverCount, phDrivers, &driverTypeDesc);
  check_error(errno, "zeInitDrivers");

  // Device who will be selected
  ze_device_handle_t hDevice = NULL;
  ze_driver_handle_t hDriver = NULL;
  
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

  //    _                      
  //   /   _  ._ _|_  _    _|_ 
  //   \_ (_) | | |_ (/_ >< |_ 
  //                           
  ze_context_handle_t hContext = NULL;
  // Create context
  ze_context_desc_t context_desc = { .stype = ZE_STRUCTURE_TYPE_CONTEXT_DESC };
  errno = zeContextCreate(hDriver, &context_desc, &hContext);
  check_error(errno, "zeContextCreate");

 //    _                                _                  
 //  /   _  ._ _  ._ _   _. ._   _|   / \      _       _  
 //  \_ (_) | | | | | | (_| | | (_|   \_X |_| (/_ |_| (/_ 
 //                                                       

  // Command queue
  ze_command_queue_desc_t commandQueueDesc = {
    .stype = ZE_STRUCTURE_TYPE_COMMAND_QUEUE_DESC,
    .ordinal = 0,
    .mode = ZE_COMMAND_QUEUE_MODE_DEFAULT,
    .priority = ZE_COMMAND_QUEUE_PRIORITY_NORMAL
  };
  
  ze_command_queue_handle_t hCommandQueue;
  errno = zeCommandQueueCreate(hContext, hDevice, &commandQueueDesc, &hCommandQueue);
  check_error(errno, "zeCommandQueueCreate");

 //   _                                           
 //  /   _  ._ _  ._ _   _. ._   _|   |  o  _ _|_ 
 //  \_ (_) | | | | | | (_| | | (_|   |_ | _>  |_ 
 //                                               

  ze_command_list_desc_t commandListDesc = {
    .stype = ZE_STRUCTURE_TYPE_COMMAND_LIST_DESC,
    .commandQueueGroupOrdinal = 0
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
    .stype = ZE_STRUCTURE_TYPE_MODULE_DESC,
    .format = ZE_MODULE_FORMAT_NATIVE,
    .inputSize = program_size,
    .pInputModule = program_file
  };

  ze_module_handle_t hModule;
  errno = zeModuleCreate(hContext, hDevice, &moduleDesc, &hModule, NULL);
  check_error(errno, "zeModuleCreate");
                     
  //   |/  _  ._ ._   _  | 
  //   |\ (/_ |  | | (/_ | 
  //                       

  ze_kernel_desc_t kernelDesc = {
    .stype = ZE_STRUCTURE_TYPE_KERNEL_DESC,
    .pKernelName = "hello_world"
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
  
  ze_group_count_t launchArgs = { .groupCountX = numGroupsX,
    .groupCountY = 1,
    .groupCountZ = 1 };
  
  // Append launch kernel
  // Launch NUM_THREADS using the same 'hCommandList'.
  // We use a mutex to sync the Happening inside each command queue 
  pthread_t threads[NUM_THREADS];
  struct s_thread_data mydata = {hCommandList, hKernel, launchArgs};

  for(long t=0; t<NUM_THREADS; t++){
    int rc = pthread_create(&threads[t], NULL, myThreadFun, (void *) &mydata);
    if (rc) exit_msg("pthread_create");
  }
   
  // Sync the Thread
  for(long t=0; t<NUM_THREADS; t++){
    int rc = pthread_join(threads[t], NULL);
    if (rc) exit_msg("pthread_join") ;
  }


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
