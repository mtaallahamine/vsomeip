#include <iomanip>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <thread>

#include <vsomeip/vsomeip.hpp>

#define FIRST_SAMPLE_SERVICE_ID 0x1234
#define FIRST_SAMPLE_INSTANCE_ID 0x5678
#define SAMPLE_EVENTGROUP_ID 0x4465
#define FIRST_SAMPLE_EVENT_ID 0x8778

#define SAMPLE_METHOD_ID  0x1421

#define SECOND_SAMPLE_SERVICE_ID 0x0248
#define SECOND_SAMPLE_INSTANCE_ID 0x5422
#define SECOND_SAMPLE_EVENT_ID 0x8777

#define THIRD_SAMPLE_SERVICE_ID 0x0233
#define THIRD_SAMPLE_INSTANCE_ID 0x5366

std::shared_ptr<vsomeip::application> app;
vsomeip::service_t _load_service_id;
vsomeip::service_t _load_instance_id;

//subscribe on the available event 
void subscribe_event(vsomeip::instance_t _event_id) {
  std::cout << "event subscription on going.." << std::endl;
  std::set<vsomeip::eventgroup_t> its_groups;
  its_groups.insert(SAMPLE_EVENTGROUP_ID);
  if(_event_id==FIRST_SAMPLE_EVENT_ID){
    std::cout << "CLIENT : Subscribe on event 1 " << std::endl;
    app->request_event(FIRST_SAMPLE_SERVICE_ID, FIRST_SAMPLE_INSTANCE_ID, _event_id, its_groups, vsomeip::event_type_e::ET_FIELD);
    app->subscribe(FIRST_SAMPLE_SERVICE_ID, FIRST_SAMPLE_INSTANCE_ID, SAMPLE_EVENTGROUP_ID);
  }else if(_event_id==SECOND_SAMPLE_EVENT_ID){
    std::cout << "CLIENT : Subscribe on event 2 " << std::endl;
    app->request_event(SECOND_SAMPLE_SERVICE_ID, SECOND_SAMPLE_INSTANCE_ID, _event_id, its_groups, vsomeip::event_type_e::ET_FIELD);
    app->subscribe(SECOND_SAMPLE_SERVICE_ID, SECOND_SAMPLE_INSTANCE_ID, SAMPLE_EVENTGROUP_ID);
  }
}
void send_message() {
  std::cout << "CLIENT: send Request message" << std::endl;
  std::shared_ptr< vsomeip::message > request;
  request = vsomeip::runtime::get()->create_request();
  request->set_service(THIRD_SAMPLE_SERVICE_ID);
  request->set_instance(THIRD_SAMPLE_INSTANCE_ID);
  request->set_method(SAMPLE_METHOD_ID);
  std::shared_ptr< vsomeip::payload > its_payload = vsomeip::runtime::get()->create_payload();
  std::vector< vsomeip::byte_t > its_payload_data;
  for (vsomeip::byte_t i=0; i<10; i++) {
      its_payload_data.push_back(i % 256);
  }
  its_payload->set_data(its_payload_data);
  request->set_payload(its_payload);
  app->send(request); 
}
//received callback :: request/response service
/*void on_response_message(const std::shared_ptr<vsomeip::message> &_response) {
  std::shared_ptr<vsomeip::payload> its_payload = _response->get_payload();
  vsomeip::length_t l = its_payload->get_length();
  // Get payload
  std::stringstream ss;
  for (vsomeip::length_t i=0; i<l; i++) {
     ss << std::setw(2) << std::setfill('0') << std::hex
        << (int)*(its_payload->get_data()+i) << " ";
  }
  std::cout << "CLIENT: Received message with Client/Session ["
      << std::setw(4) << std::setfill('0') << std::hex << _response->get_client() << "/"
      << std::setw(4) << std::setfill('0') << std::hex << _response->get_session() << "] "
      << ss.str() << std::endl;
}*/
//received callback :: notify/subscribe service
void on_message(const std::shared_ptr<vsomeip::message> &_response) {
   if((_load_service_id==FIRST_SAMPLE_SERVICE_ID && _load_instance_id==FIRST_SAMPLE_INSTANCE_ID) || 
   (_load_service_id==SECOND_SAMPLE_SERVICE_ID && _load_instance_id==SECOND_SAMPLE_INSTANCE_ID)){
      std::stringstream its_message;
      its_message << "CLIENT: received a notification for event ["
            << std::setw(4) << std::setfill('0') << std::hex
            << _response->get_service() << "."
            << std::setw(4) << std::setfill('0') << std::hex
            << _response->get_instance() << "."
            << std::setw(4) << std::setfill('0') << std::hex
            << _response->get_method() << "] to Client/Session ["
            << std::setw(4) << std::setfill('0') << std::hex
            << _response->get_client() << "/"
            << std::setw(4) << std::setfill('0') << std::hex
            << _response->get_session()
            << "] = ";
     std::shared_ptr<vsomeip::payload> its_payload = _response->get_payload();
     its_message << "(" << std::dec << its_payload->get_length() << ") ";
     for (uint32_t i = 0; i < its_payload->get_length(); ++i)
        its_message << std::hex << std::setw(2) << std::setfill('0')
            << (int) its_payload->get_data()[i] << " ";
    std::cout << its_message.str() << std::endl;
  }else if(_load_service_id==THIRD_SAMPLE_SERVICE_ID && _load_instance_id==THIRD_SAMPLE_INSTANCE_ID){
     std::shared_ptr<vsomeip::payload> its_payload = _response->get_payload();
     vsomeip::length_t l = its_payload->get_length();
    // Get payload
    std::stringstream ss;
    for (vsomeip::length_t i=0; i<l; i++) {
      ss << std::setw(2) << std::setfill('0') << std::hex
        << (int)*(its_payload->get_data()+i) << " ";
      }
    std::cout << "CLIENT: Received message with Client/Session ["
      << std::setw(4) << std::setfill('0') << std::hex << _response->get_client() << "/"
      << std::setw(4) << std::setfill('0') << std::hex << _response->get_session() << "] "
      << ss.str() << std::endl;
  }else{
    std::cout << "no service loaded !!!" <<std::endl;
  }

}

void on_availability(vsomeip::service_t _service, vsomeip::instance_t _instance, bool _is_available) {
    std::cout << "Service ["
            << std::setw(4) << std::setfill('0') << std::hex << _service << "." << _instance
            << "] is " << (_is_available ? "available." : "NOT available.")  << std::endl;
    if (_is_available)
    { 
      if(_service==FIRST_SAMPLE_SERVICE_ID && _instance==FIRST_SAMPLE_INSTANCE_ID ){
        _load_service_id=_service;
        _load_instance_id=_instance;
        std::cout << "service event[1] : is available" <<std::endl;
        subscribe_event(FIRST_SAMPLE_EVENT_ID);
      }else if(_service==SECOND_SAMPLE_SERVICE_ID && _instance==SECOND_SAMPLE_INSTANCE_ID){
        _load_service_id=_service;
        _load_instance_id=_instance;
        std::cout << "service event[2] : is available" <<std::endl;
        subscribe_event(SECOND_SAMPLE_EVENT_ID);
      }else if(_service==THIRD_SAMPLE_SERVICE_ID && _instance==THIRD_SAMPLE_INSTANCE_ID){
        _load_service_id=_service;
        _load_instance_id=_instance;
        std::cout << "service request : is available" <<std::endl;
        send_message();    
      }
    } 
}
int main(){
    app = vsomeip::runtime::get()->create_application("client");
    app->init();
    //callback subscription for first service : event 1
    app->register_availability_handler(FIRST_SAMPLE_SERVICE_ID, FIRST_SAMPLE_INSTANCE_ID, on_availability);
    app->request_service(vsomeip::ANY_SERVICE, vsomeip::ANY_INSTANCE);
    app->register_message_handler(FIRST_SAMPLE_SERVICE_ID, FIRST_SAMPLE_INSTANCE_ID, vsomeip::ANY_METHOD, on_message);
    //callback subscription for second service : event 2
    app->register_availability_handler(SECOND_SAMPLE_SERVICE_ID, SECOND_SAMPLE_INSTANCE_ID, on_availability);
    app->request_service(SECOND_SAMPLE_SERVICE_ID, SECOND_SAMPLE_INSTANCE_ID);
    app->register_message_handler(SECOND_SAMPLE_SERVICE_ID, SECOND_SAMPLE_INSTANCE_ID, vsomeip::ANY_METHOD, on_message);
    //callback for third service : request service
    app->register_availability_handler(THIRD_SAMPLE_SERVICE_ID, THIRD_SAMPLE_INSTANCE_ID, on_availability);
    app->request_service(THIRD_SAMPLE_SERVICE_ID, THIRD_SAMPLE_INSTANCE_ID);
    app->register_message_handler(THIRD_SAMPLE_SERVICE_ID, THIRD_SAMPLE_INSTANCE_ID, SAMPLE_METHOD_ID, on_message);
    app->start();
}