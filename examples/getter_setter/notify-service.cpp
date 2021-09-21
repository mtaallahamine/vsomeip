#include <iomanip>
#include <iostream>
#include <sstream>

#include <vsomeip/vsomeip.hpp>

#define SAMPLE_SERVICE_ID 0x1234
#define SAMPLE_INSTANCE_ID 0x5678
#define SAMPLE_METHOD_ID 0x0421
#define SAMPLE_EVENTGROUP_ID 0x4465
#define SAMPLE_EVENT_ID 0x8778

std::shared_ptr<vsomeip::application> app;

void on_message(const std::shared_ptr<vsomeip::message> &_request) {
    // Get payload
    std::stringstream its_message;
    its_message << "SERVICE: received a notification for event ["
            << std::setw(4) << std::setfill('0') << std::hex
            << _request->get_service() << "."
            << std::setw(4) << std::setfill('0') << std::hex
            << _request->get_instance() << "."
            << std::setw(4) << std::setfill('0') << std::hex
            << _request->get_method() << "] to Client/Session ["
            << std::setw(4) << std::setfill('0') << std::hex
            << _request->get_client() << "/"
            << std::setw(4) << std::setfill('0') << std::hex
            << _request->get_session()
            << "] = ";
    std::shared_ptr<vsomeip::payload> its_payload = _request->get_payload();
    its_message << "(" << std::dec << its_payload->get_length() << ") ";
    for (uint32_t i = 0; i < its_payload->get_length(); ++i)
        its_message << std::hex << std::setw(2) << std::setfill('0')
            << (int) its_payload->get_data()[i] << " ";
    std::cout << its_message.str() << std::endl;

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
const vsomeip::byte_t its_data[] = { 0x10 };
std::shared_ptr<vsomeip::payload> payload_data = vsomeip::runtime::get()->create_payload();
payload_data->set_data(its_data, sizeof(its_data));
std::set<vsomeip::eventgroup_t> its_groups;

app = vsomeip::runtime::get()->create_application("notify-service");
app->init();
app->register_message_handler(SAMPLE_SERVICE_ID, SAMPLE_INSTANCE_ID, SAMPLE_METHOD_ID, on_message);
app->offer_event(SAMPLE_SERVICE_ID, SAMPLE_INSTANCE_ID, SAMPLE_EVENT_ID, its_groups, vsomeip::event_type_e::ET_FIELD);
app->notify(SAMPLE_SERVICE_ID, SAMPLE_INSTANCE_ID, SAMPLE_EVENT_ID, payload_data);
app->start();

}