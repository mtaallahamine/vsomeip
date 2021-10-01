#include "main.h"

static uint16_t main_clientServiceID = 0x1234u;

static uint16_t main_clientInstanceID = 0x5678u;

static uint16_t main_eventID = 0x8778u;

static uint16_t main_eventGroupID = 0x4465u;

void serviceAvailableHandler(uint16_t serviceID, uint16_t serviceInstanceID, bool isAvailable)
{
  if (isAvailable) 
  {
    printf("service is Available");
    if (serviceID == main_clientServiceID && serviceInstanceID == main_clientInstanceID) 
    {
      subscribeEvent(serviceID, serviceInstanceID, main_eventGroupID, main_eventID);
    }
  }
  else
  {
    printf("no service available");
  }
}

void eventNotificationHandler(uint16_t serviceID, uint16_t serviceInstanceID, uint16_t eventID, uint8_t *data, size_t dataLength)
{
  printf("event notification Received \n");
  printf("serviceID: [%d] instanceID: [%d] eventID: [%d] \n", serviceID, serviceInstanceID, eventID);
}

int32_t main(int32_t argc, char *argv[])
{
  initializeService("someipClientDemo");
  printf("************ initialize someipClientDemo **************");
  startServices();
  printf("********* someipClient Demo application started ************** ");
  return 0;
}
