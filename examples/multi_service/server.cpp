#include <iomanip>
#include <iostream>
#include <sstream>

#include <vsomeip/vsomeip.hpp>

#define SAMPLE_SERVICE_ID 0x1234
#define SAMPLE_INSTANCE_ID 0x5678
#define SAMPLE_EVENTGROUP_ID 0x4465
#define SAMPLE_EVENT_ID 0x8778

#define SAMPLE_METHOD_ID  0x1421

#define SECOND_SAMPLE_EVENT_ID 0x8777
#define THIRD_SAMPLE_EVENT_ID 0x8776

#define OTHER_SAMPLE_SERVICE_ID 0x0248
#define OTHER_SAMPLE_INSTANCE_ID 0x5422

#define THIRD_SAMPLE_SERVICE_ID 0x0233
#define THIRD_SAMPLE_INSTANCE_ID 0x5366


std::shared_ptr<vsomeip::application> app;
void on_message(const std::shared_ptr<vsomeip::message> &_request) {

    std::shared_ptr<vsomeip::payload> its_payload = _request->get_payload();
    vsomeip::length_t l = its_payload->get_length();

    // Get payload
    std::stringstream ss;
    for (vsomeip::length_t i=0; i<l; i++) {
       ss << std::setw(2) << std::setfill('0') << std::hex
          << (int)*(its_payload->get_data()+i) << " ";
    }
    std::cout << "SERVER: Received message with Client/Session ["
        << std::setw(4) << std::setfill('0') << std::hex << _request->get_client() << "/"
        << std::setw(4) << std::setfill('0') << std::hex << _request->get_session() << "] "
        << ss.str() << std::endl;

    // Create response
    std::shared_ptr<vsomeip::message> its_response = vsomeip::runtime::get()->create_response(_request);
    its_payload = vsomeip::runtime::get()->create_payload();
    std::vector<vsomeip::byte_t> its_payload_data;
    for (int i=9; i>=0; i--) {
        its_payload_data.push_back(i % 256);
    }
    its_payload->set_data(its_payload_data);
    its_response->set_payload(its_payload);
    app->send(its_response);
}
int main(){
// set first event
const vsomeip::byte_t its_data1[] = { 0x10 };
std::shared_ptr<vsomeip::payload> payload_notif1;
payload_notif1 = vsomeip::runtime::get()->create_payload();
payload_notif1->set_data(its_data1, sizeof(its_data1));
//set second event
const vsomeip::byte_t its_data2[] = { 0x20 };
std::shared_ptr<vsomeip::payload> payload_notif2;
payload_notif2 = vsomeip::runtime::get()->create_payload();
payload_notif2->set_data(its_data2, sizeof(its_data2));
//create and itialize application
app = vsomeip::runtime::get()->create_application("server");
app->init();
std::set<vsomeip::eventgroup_t> its_groups;
its_groups.insert(SAMPLE_EVENTGROUP_ID);
//offer first service : offer event 1
app->offer_service(SAMPLE_SERVICE_ID, SAMPLE_INSTANCE_ID);
app->offer_event(SAMPLE_SERVICE_ID, SAMPLE_INSTANCE_ID, SAMPLE_EVENT_ID, its_groups, vsomeip::event_type_e::ET_FIELD);
app->notify(SAMPLE_SERVICE_ID, SAMPLE_INSTANCE_ID, SAMPLE_EVENT_ID, payload_notif1);
//offer second service : offer event 2
app->offer_service(OTHER_SAMPLE_SERVICE_ID, OTHER_SAMPLE_INSTANCE_ID);
app->offer_event(OTHER_SAMPLE_SERVICE_ID, OTHER_SAMPLE_INSTANCE_ID, SECOND_SAMPLE_EVENT_ID, its_groups, vsomeip::event_type_e::ET_FIELD);
app->notify(OTHER_SAMPLE_SERVICE_ID, OTHER_SAMPLE_INSTANCE_ID, SECOND_SAMPLE_EVENT_ID, payload_notif2);
//offer third service : offer request service
app->register_message_handler(THIRD_SAMPLE_SERVICE_ID, THIRD_SAMPLE_INSTANCE_ID, SAMPLE_METHOD_ID, on_message);
app->offer_service(THIRD_SAMPLE_SERVICE_ID, THIRD_SAMPLE_INSTANCE_ID);
//start application
app->start();
}