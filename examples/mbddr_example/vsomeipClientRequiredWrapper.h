#ifndef vsomeipClientRequiredWrapper_h
#define vsomeipClientRequiredWrapper_h
#ifdef __cplusplus
extern "C" {
#endif

void ServiceAvailableHandler(uint16_t serviceID, uint16_t serviceInstanceID, bool isAvailable);
void responseReceivedHandler(uint16_t serviceID, uint16_t serviceInstanceID, uint16_t methodID, uint8_t* data, size_t dataLength);
void eventNotificationHandler(uint16_t serviceID, uint16_t serviceInstanceID, uint16_t eventID, uint8_t* data, size_t dataLength);

#ifdef __cplusplus
} /* extern "C" */
#endif
#endif
