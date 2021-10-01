#include <vsomeipCommonWrapper.h>

#ifndef vsomeipClientWrapper_h
#define vsomeipClientWrapper_h
#ifdef __cplusplus
extern "C" {
#endif

void requestService(uint16_t  serviceID, uint16_t serviceInstanceID, uint16_t methodID);
void sendRequest(uint16_t serviceID, uint16_t serviceInstanceID, uint16_t methodID,uint8_t* data, size_t dataLength);
void subscribeEvent(uint16_t serviceID, uint16_t serviceInstanceID, uint16_t eventGroupID, uint16_t eventID);
void onServiceAvailable(vsomeip::service_t serviceID, vsomeip::instance_t serviceInstanceID, bool isAvailable);
void onResponseReceived(const std::shared_ptr<vsomeip::message> &response);
void onNotificationReceived(const std::shared_ptr<vsomeip::message> &response);
void releaseRequestedServices();

#ifdef __cplusplus
} /* extern "C" */
#endif
#endif

