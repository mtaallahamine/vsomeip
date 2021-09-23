# Copyright (C) 2015-2017 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

cmake_minimum_required (VERSION 2.8.7)

# Examples
add_executable(subscribe-client subscribe-client.cpp)
target_link_libraries(subscribe-client ${VSOMEIP_NAME} ${Boost_LIBRARIES} ${DL_LIBRARY})

add_executable(notify-service notify-service.cpp)
target_link_libraries(notify-service ${VSOMEIP_NAME} ${Boost_LIBRARIES} ${DL_LIBRARY})


install (
    TARGETS subscribe-client notify-service
    RUNTIME DESTINATION "${INSTALL_BIN_DIR}" COMPONENT bin
)