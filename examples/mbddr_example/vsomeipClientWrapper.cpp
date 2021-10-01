#include <vsomeipClientWrapper.h>
#include <vsomeipClientRequiredWrapper.h>

void requestService(uint16_t serviceID, uint16_t serviceInstanceID, uint16_t methodID){
    someipApplication->register_availability_handler(serviceID, serviceInstanceID, onServiceAvailable);
    someipApplication->register_message_handler(serviceID, serviceInstanceID, vsomeip::ANY_METHOD, onResponseReceived);
    someipApplication->request_service(serviceID, serviceInstanceID, methodID);
}

void sendRequest(uint16_t serviceID, uint16_t serviceInstanceID, uint16_t methodID, uint8_t* data, size_t dataLength){
    std::shared_ptr< vsomeip::message > request=vsomeip::runtime::get()->create_request();
    request->set_service(serviceID);
    request->set_instance(serviceInstanceID);
    request->set_method(methodID);   
    std::shared_ptr< vsomeip::payload > payload = vsomeip::runtime::get()->create_payload();
    payload->set_data(data,dataLength);
    request->set_payload(payload);
    someipApplication->send(request); 
}

void subscribeEvent(uint16_t serviceID, uint16_t serviceInstanceID, uint16_t eventGroupID, uint16_t eventID){
    std::set<vsomeip::eventgroup_t> eventGroup;
    eventGroup.insert(eventGroupID);
    someipApplication->register_availability_handler(serviceID, serviceInstanceID, onServiceAvailable);
    someipApplication->register_message_handler(serviceID, serviceInstanceID, vsomeip::ANY_METHOD, onNotificationReceived);
    someipApplication->request_event(serviceID, serviceInstanceID, eventID, eventGroup, vsomeip::event_type_e::ET_FIELD);
    someipApplication->subscribe(serviceID, serviceInstanceID, eventGroupID);
}

void onServiceAvailable(vsomeip::service_t serviceID, vsomeip::instance_t serviceInstanceID, bool isAvailable){
    ServiceAvailableHandler(serviceID, serviceInstanceID, isAvailable);
}

void onResponseReceived(const std::shared_ptr<vsomeip::message> &response){
    std::shared_ptr<vsomeip::payload> payload = response->get_payload();
    responseReceivedHandler(response->get_service(), response->get_instance(), response->get_method(), payload->get_data(), payload->get_length());
}

void onNotificationReceived(const std::shared_ptr<vsomeip::message> &response){
    std::shared_ptr<vsomeip::payload> payload = response->get_payload();
    eventNotificationHandler(response->get_service(), response->get_instance(), response->get_method(), payload->get_data(), payload->get_length());
}

void releaseRequestedServices(){
    someipApplication->release_service(vsomeip::ANY_SERVICE, vsomeip::ANY_INSTANCE);
}
