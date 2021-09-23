#include <iomanip>
#include <iostream>
#include <sstream>

#include <vsomeip/vsomeip.hpp>

#define SAMPLE_SERVICE_ID 0x1234
#define SAMPLE_INSTANCE_ID 0x5678
#define SAMPLE_EVENTGROUP_ID 0x4465
#define SAMPLE_EVENT_ID 0x8778

#define SAMPLE_METHOD_ID 0x0421

std::shared_ptr<vsomeip::application> app;
std::shared_ptr<vsomeip::payload> payload_;
void on_get(const std::shared_ptr<vsomeip::message> &_message) {
    std::cout << "********* CLIENT on_get method *************" <<std::endl;
        std::shared_ptr<vsomeip::message> its_response
            = vsomeip::runtime::get()->create_response(_message);
        {
            its_response->set_payload(payload_);
        }
        app_->send(its_response);
    }
    void on_set(const std::shared_ptr<vsomeip::message> &_message) {
    std::cout << "********* CLIENT on_set method *************" <<std::endl;
        std::shared_ptr<vsomeip::message> its_response
            = vsomeip::runtime::get()->create_response(_message);
        {
            payload_ = _message->get_payload();
            its_response->set_payload(payload_);
        }

        app_->send(its_response);
        app_->notify(SAMPLE_SERVICE_ID, SAMPLE_INSTANCE_ID,
                     SAMPLE_EVENT_ID, payload_);
    }
int main(){
const vsomeip::byte_t its_data[] = { 0x10 };
std::shared_ptr<vsomeip::payload> payload_notif;
payload_notif = vsomeip::runtime::get()->create_payload();
payload_notif->set_data(its_data, sizeof(its_data));
app = vsomeip::runtime::get()->create_application("notify-service");
app->init();
app->register_message_handler(SAMPLE_SERVICE_ID, SAMPLE_INSTANCE_ID, SAMPLE_METHOD_ID, on_get);
app->register_message_handler(SAMPLE_SERVICE_ID, SAMPLE_INSTANCE_ID, SAMPLE_METHOD_ID, on_set);
app->offer_service(SAMPLE_SERVICE_ID, SAMPLE_INSTANCE_ID);
std::set<vsomeip::eventgroup_t> its_groups;
its_groups.insert(SAMPLE_EVENTGROUP_ID);
app->offer_event(SAMPLE_SERVICE_ID, SAMPLE_INSTANCE_ID, SAMPLE_EVENT_ID, its_groups, vsomeip::event_type_e::ET_FIELD);
app->notify(SAMPLE_SERVICE_ID, SAMPLE_INSTANCE_ID, SAMPLE_EVENT_ID, payload_notif);
app->start();
}