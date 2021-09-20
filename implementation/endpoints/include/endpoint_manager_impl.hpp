// Copyright (C) 2014-2017 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#ifndef VSOMEIP_V3_ENDPOINT_MANAGER_IMPL_HPP_
#define VSOMEIP_V3_ENDPOINT_MANAGER_IMPL_HPP_

#include "../include/endpoint_manager_base.hpp"

namespace vsomeip_v3 {

class routing_host;

class endpoint_manager_impl : public endpoint_manager_base {
public:
    endpoint_manager_impl(routing_manager_base* const _rm,
                          boost::asio::io_service& _io,
                          const std::shared_ptr<configuration>& _configuration);
    ~endpoint_manager_impl() = default;

    std::shared_ptr<endpoint> find_or_create_remote_client(service_t _service,
                                                           instance_t _instance,
                                                           bool _reliable);

    void find_or_create_remote_client(service_t _service, instance_t _instance);
    void is_remote_service_known(
            service_t _service, instance_t _instance, major_version_t _major,
            minor_version_t _minor,
            const boost::asio::ip::address &_reliable_address,
            uint16_t _reliable_port, bool* _reliable_known,
            const boost::asio::ip::address &_unreliable_address,
            uint16_t _unreliable_port, bool* _unreliable_known) const;
    void add_remote_service_info(
            service_t _service, instance_t _instance,
            const std::shared_ptr<endpoint_definition>& _ep_definition);
    void add_remote_service_info(
            service_t _service, instance_t _instance,
            const std::shared_ptr<endpoint_definition>& _ep_definition_reliable,
            const std::shared_ptr<endpoint_definition>& _ep_definition_unreliable);
    void clear_remote_service_info(service_t _service, instance_t _instance,
                                   bool _reliable);

    std::shared_ptr<endpoint> create_server_endpoint(uint16_t _port,
                                                     bool _reliable,
                                                     bool _start);

    std::shared_ptr<endpoint> find_server_endpoint(uint16_t _port,
                                                   bool _reliable) const;

    std::shared_ptr<endpoint> find_or_create_server_endpoint(
            uint16_t _port, bool _reliable, bool _start, service_t _service,
            instance_t _instance, bool &_is_found, bool _is_multicast = false);
    bool remove_server_endpoint(uint16_t _port, bool _reliable);


    void clear_client_endpoints(service_t _service, instance_t _instance,
                                bool _reliable);
    void find_or_create_multicast_endpoint(
            service_t _service, instance_t _instance,
            const boost::asio::ip::address &_sender,
            const boost::asio::ip::address &_address, uint16_t _port);
    void clear_multicast_endpoints(service_t _service, instance_t _instance);

    bool supports_selective(service_t _service, instance_t _instance) const;

    void print_status() const;

    std::shared_ptr<local_server_endpoint_impl> create_local_server(
            bool* _is_socket_activated,
            const std::shared_ptr<routing_host>& _routing_host);

    instance_t find_instance(service_t _service,
                             endpoint* const _endpoint) const;
    instance_t find_instance_multicast(service_t _service,
            const boost::asio::ip::address &_sender) const;

    bool remove_instance(service_t _service, endpoint* const _endpoint);
    bool remove_instance_multicast(service_t _service, instance_t _instance);


    // endpoint_host interface
    void on_connect(std::shared_ptr<endpoint> _endpoint);
    void on_disconnect(std::shared_ptr<endpoint> _endpoint);
    void on_error(const byte_t *_data, length_t _length,
                          endpoint* const _receiver,
                          const boost::asio::ip::address &_remote_address,
                          std::uint16_t _remote_port);
    void release_port(uint16_t _port, bool _reliable);

    // Statistics
    void log_client_states() const;
    void log_server_states() const;

private:
    std::shared_ptr<endpoint> find_remote_client(service_t _service,
                                                 instance_t _instance,
                                                 bool _reliable);
    std::shared_ptr<endpoint> create_remote_client(service_t _service,
                                                   instance_t _instance,
                                                   bool _reliable);
    std::shared_ptr<endpoint> create_client_endpoint(
            const boost::asio::ip::address &_address, uint16_t _local_port,
            uint16_t _remote_port, bool _reliable);

private:
    mutable std::recursive_mutex endpoint_mutex_;
    // Client endpoints for remote services
    std::map<service_t, std::map<instance_t,
            std::map<bool, std::shared_ptr<endpoint_definition>>>> remote_service_info_;

    typedef std::map<service_t, std::map<instance_t,
                std::map<bool, std::shared_ptr<endpoint>>>> remote_services_t;
    remote_services_t remote_services_;

    typedef std::map<boost::asio::ip::address, std::map<uint16_t,
                std::map<bool, std::shared_ptr<endpoint>>>> client_endpoints_by_ip_t;
    client_endpoints_by_ip_t client_endpoints_by_ip_;

    std::map<service_t, std::map<endpoint *, instance_t> > service_instances_;
    std::map<service_t, std::map<boost::asio::ip::address, instance_t> > service_instances_multicast_;

    std::map<bool, std::set<uint16_t>> used_client_ports_;
    std::mutex used_client_ports_mutex_;

    // Server endpoints for local services
    typedef std::map<uint16_t, std::map<bool, std::shared_ptr<endpoint>>> server_endpoints_t;
    server_endpoints_t server_endpoints_;

    // Multicast endpoint info (notifications)
    std::map<service_t, std::map<instance_t,
                    std::shared_ptr<endpoint_definition>>> multicast_info;
};

} // namespace vsomeip_v3

#endif // VSOMEIP_V3_ENDPOINT_MANAGER_IMPL_HPP_
