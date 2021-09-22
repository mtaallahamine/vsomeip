#include <iomanip>
#include <iostream>
#include <sstream>

#include <vsomeip/vsomeip.hpp>

#define SAMPLE_SERVICE_ID 0x1234
#define SAMPLE_INSTANCE_ID 0x5678
#define SAMPLE_EVENTGROUP_ID 0x4465
#define SAMPLE_EVENT_ID 0x8778

std::shared_ptr<vsomeip::application> app;

int main(){
const vsomeip::byte_t its_data[] = { 0x10 };
std::shared_ptr<vsomeip::payload> payload_notif;
payload_notif = vsomeip::runtime::get()->create_payload();
payload_notif->set_data(its_data, sizeof(its_data));
app = vsomeip::runtime::get()->create_application("notify-multi-service");
app->init();
app->offer_service(SAMPLE_SERVICE_ID, SAMPLE_INSTANCE_ID);
std::set<vsomeip::eventgroup_t> its_groups;
its_groups.insert(SAMPLE_EVENTGROUP_ID);
app->offer_event(SAMPLE_SERVICE_ID, SAMPLE_INSTANCE_ID, SAMPLE_EVENT_ID, its_groups, vsomeip::event_type_e::ET_FIELD);
app->notify(SAMPLE_SERVICE_ID, SAMPLE_INSTANCE_ID, SAMPLE_EVENT_ID, payload_notif);
app->start();
}