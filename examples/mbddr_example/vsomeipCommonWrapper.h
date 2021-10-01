#include <vsomeip/vsomeip.hpp>

#ifndef vsomeipCommonWrapper_h
#define vsomeipCommonWrapper_h

#ifdef __cplusplus
extern "C" {
#endif

std::shared_ptr<vsomeip::application>  someipApplication;

bool initializeService(char* name);
void startServices();
void stopServices();
void onApplicationRegistered(vsomeip::state_type_e  state);

#ifdef __cplusplus
} /* extern "C" */
#endif
#endif




