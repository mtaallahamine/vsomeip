#include <vsomeipCommonWrapper.h>
#include <vsomeipCommonRequiredWrapper.h>

bool initializeService(char* name){
    someipApplication = vsomeip::runtime::get()->create_application(name);
    someipApplication->register_state_handler(onApplicationRegistered);
    return someipApplication->init();
}

void startServices(){
    someipApplication->start();
}

void stopServices(){
    someipApplication->clear_all_handler();
    someipApplication->stop();
}

void onApplicationRegistered(vsomeip::state_type_e  state){
    applicationRegisterHandler((uint8_t)state);
}
