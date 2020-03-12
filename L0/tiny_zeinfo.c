#include <stdlib.h>

#include "stdio.h"
#include "ze_api.h"
#include "zet_api.h"

#include "./ze_utils.h"
int main()
{
  ze_result_t errno;
  //  _              _                      _
  // |_) |  _. _|_ _|_ _  ._ ._ _    ()    | \  _     o  _  _
  // |   | (_|  |_  | (_) |  | | |   (_X   |_/ (/_ \/ | (_ (/_
  //
  printf(">>> Initializing OpenCL Platform and Device...\n");

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

  for(uint32_t driver_idx = 0; driver_idx < driverCount; driver_idx++) {

    ze_driver_handle_t driver = phDrivers[driver_idx];

    ze_driver_properties_t driver_properties ;
    errno = zeDriverGetProperties(driver, &driver_properties);
    check_error(errno, "zeDriverGetProperties");

    printf("Platforn #%d: driver_version %u\n", driver_idx, driver_properties.driverVersion);

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
        printf("Device #%d %s\n ", device_idx, device_properties.name);
        printf("  numSlices %u\n ", device_properties.numSlices);
        printf("  numSubslicesPerSlice %u\n ", device_properties.numSubslicesPerSlice);
        printf("  numEUsPerSubslice %u\n ", device_properties.numEUsPerSubslice);
        printf("  physicalEUSimdWidth %u\n ", device_properties.physicalEUSimdWidth);
        printf("  numThreadsPerEU %u\n ", device_properties.numThreadsPerEU);

        // |\/|  _ _|_ ._ o  _
        // |  | (/_ |_ |  | (_
        //
        
        /* Not yet working
        // Obtain available metric groups for the specific device
        uint32_t metricGroupCount = 0;
        errno = zetMetricGroupGet( phDevices[device_idx], &metricGroupCount, NULL);
        check_error(errno, "zetMetricGroupGet");

        zet_metric_group_handle_t* phMetricGroups = malloc(metricGroupCount * sizeof(zet_metric_group_handle_t));
        errno = zetMetricGroupGet( phDevices[device_idx], &metricGroupCount, phMetricGroups );
        check_error(errno, "zetMetricGroupGet");

        // Iterate over all metric groups available
        for(int i = 0; i < metricGroupCount; i++ ) {
          // Get metric group under index 'i' and its properties
          zet_metric_group_properties_t metricGroupProperties;
          zetMetricGroupGetProperties( phMetricGroups[i], &metricGroupProperties );
          printf("Metric Group: %s\n", metricGroupProperties.name);
        } 
        free(phMetricGroups);
        */
    }

    free(phDevices);
  }
    
  free(phDrivers);
  return 0;
}
