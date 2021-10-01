# Copyright (C) 2015-2017 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

cmake_minimum_required (VERSION 2.8.7)

add_library(vsomeip_main INTERFACE)
target_sources(vsomeip_main INTERFACE
    "${CMAKE_CURRENT_SOURCE_DIR}/main.h" "${CMAKE_CURRENT_SOURCE_DIR}/vsomeipClientRequiredWrapper.h" 
    "${CMAKE_CURRENT_SOURCE_DIR}/vsomeipClientWrapper.h" "${CMAKE_CURRENT_SOURCE_DIR}/vsomeipCommonRequiredWrapper.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/vsomeipCommonWrapper.h"
)
target_include_directories(vsomeip_main INTERFACE
        "${CMAKE_CURRENT_SOURCE_DIR}"
)

# Examples
add_executable(main main.c vsomeipClientWrapper.cpp vsomeipCommonWrapper.cpp)
target_link_libraries(main vsomeip_main ${VSOMEIP_NAME} ${Boost_LIBRARIES} ${DL_LIBRARY})


install (
    TARGETS main
    RUNTIME DESTINATION "${INSTALL_BIN_DIR}" COMPONENT bin
)