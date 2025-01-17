// Copyright (C) 2020 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#ifndef VSOMEIP_V3_E2E_PROFILE04_CHECKER_HPP
#define VSOMEIP_V3_E2E_PROFILE04_CHECKER_HPP

#include "../profile04/profile_04.hpp"
#include "../profile_interface/checker.hpp"

namespace vsomeip_v3 {
namespace e2e {
namespace profile04 {

class profile_04_checker final : public e2e::profile_interface::checker {

public:
    profile_04_checker(void) = delete;

    // [SWS_E2E_00389] initialize state
    explicit profile_04_checker(const profile_config &_config) :
            config_(_config), counter_(0xffff) {}

    void check(const e2e_buffer &_buffer, instance_t _instance,
            e2e::profile_interface::check_status_t &_generic_check_status) override final;

private:
    bool verify_input(const e2e_buffer &_buffer) const;
    bool verify_counter(uint16_t _received_counter) const;

    bool read_16(const e2e_buffer &_buffer, uint16_t &_data, size_t _index) const;
    bool read_32(const e2e_buffer &_buffer, uint32_t &_data, size_t _index) const;

    std::mutex check_mutex_;

    profile_config config_;
    uint16_t counter_;
};

} // namespace profile_04
} // namespace e2e
} // namespace vsomeip_v3

#endif // VSOMEIP_V3_E2E_PROFILE04_CHECKER_HPP
