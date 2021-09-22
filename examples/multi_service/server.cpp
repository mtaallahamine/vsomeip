#include <iomanip>
#include <iostream>
#include <sstream>

#include <vsomeip/vsomeip.hpp>

#define SAMPLE_SERVICE_ID 0x1234
#define SAMPLE_INSTANCE_ID 0x5678
#define SAMPLE_EVENTGROUP_ID 0x4465
#define SAMPLE_EVENT_ID 0x8778

#define OTHER_SAMPLE_SERVICE_ID 0x0248
#define OTHER_SAMPLE_INSTANCE_ID 0x5422

std::shared_ptr<vsomeip::application> app;

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
//offer first service 
app->offer_service(SAMPLE_SERVICE_ID, SAMPLE_INSTANCE_ID);
app->offer_event(SAMPLE_SERVICE_ID, SAMPLE_INSTANCE_ID, SAMPLE_EVENT_ID, its_groups, vsomeip::event_type_e::ET_FIELD);
app->notify(SAMPLE_SERVICE_ID, SAMPLE_INSTANCE_ID, SAMPLE_EVENT_ID, payload_notif1);
//offer second service
app->offer_service(OTHER_SAMPLE_SERVICE_ID, OTHER_SAMPLE_INSTANCE_ID);
app->offer_event(OTHER_SAMPLE_SERVICE_ID, OTHER_SAMPLE_INSTANCE_ID, SAMPLE_EVENT_ID, its_groups, vsomeip::event_type_e::ET_FIELD);
app->notify(OTHER_SAMPLE_SERVICE_ID, OTHER_SAMPLE_INSTANCE_ID, SAMPLE_EVENT_ID, payload_notif2);
//start application
app->start();
}