// Copyright (C) 2017 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
#include "../include/channel_impl.hpp"
#include <vsomeip/internal/logger.hpp>

namespace vsomeip_v3 {
namespace trace {

const filter_id_t FILTER_ID_ERROR(0);

channel_impl::channel_impl(const std::string &_id, const std::string &_name)
    : id_(_id), name_(_name), current_filter_id_(1) {
}

std::string channel_impl::get_id() const {
    return id_;
}

std::string channel_impl::get_name() const {
    return name_;
}

filter_id_t channel_impl::add_filter(
        const match_t &_match, bool _is_positive) {

    // Create a filter function
    std::function<bool (service_t, instance_t, method_t)> its_filter_func;
    if (std::get<0>(_match) != ANY_SERVICE) {
        if (std::get<1>(_match) != ANY_INSTANCE) {
            if (std::get<2>(_match) != ANY_METHOD) {
                its_filter_func
                    = [_match](service_t _s, instance_t _i, method_t _m) {
                          return (std::get<0>(_match) == _s
                                  && std::get<1>(_match) == _i
                                  && std::get<2>(_match) == _m);
                      };
            } else {
                its_filter_func
                    = [_match](service_t _s, instance_t _i, method_t) {
                          return (std::get<0>(_match) == _s
                                  && std::get<1>(_match) == _i);
                      };
            }
        } else {
            if (std::get<2>(_match) != ANY_METHOD) {
                its_filter_func
                    = [_match](service_t _s, instance_t, method_t _m) {
                          return (std::get<0>(_match) == _s
                                  && std::get<1>(_match) == _m);
                      };
            } else {
                its_filter_func
                    = [_match](service_t _s, instance_t, method_t) {
                          return (std::get<0>(_match) == _s);
                      };
            }
        }
    } else {
        if (std::get<1>(_match) != ANY_INSTANCE) {
            if (std::get<2>(_match) != ANY_METHOD) {
                its_filter_func
                    = [_match](service_t, instance_t _i, method_t _m) {
                          return (std::get<1>(_match) == _i
                                  && std::get<2>(_match) == _m);
                      };
            } else {
                its_filter_func
                    = [_match](service_t, instance_t _i, method_t) {
                          return (std::get<1>(_match) == _i);
                      };
            }
        } else {
            if (std::get<2>(_match) != ANY_METHOD) {
                its_filter_func
                    = [_match](service_t, instance_t, method_t _m) {
                          return (std::get<2>(_match) == _m);
                      };
            } else {
                its_filter_func
                    = [](service_t, instance_t, method_t) {
                          return true;
                      };
            }
        }
    }

    return add_filter_intern(its_filter_func, _is_positive);
}

filter_id_t channel_impl::add_filter(
        const std::vector<match_t> &_matches, bool _is_positive) {
    bool has_service(false);
    bool has_instance(false);
    bool has_method(false);

    for (auto m : _matches) {
        if (std::get<0>(m) != ANY_SERVICE) has_service = true;
        if (std::get<1>(m) != ANY_INSTANCE) has_instance = true;
        if (std::get<2>(m) != ANY_METHOD) has_method = true;
    }

    // Create a filter function
    std::function<bool (service_t, instance_t, method_t)> its_filter_func;
    if (has_service) {
        if (has_instance) {
            if (has_method) {
                its_filter_func
                    = [_matches](service_t _s, instance_t _i, method_t _m) {
                          for (const auto &m : _matches) {
                              if ((std::get<0>(m) == _s || std::get<0>(m) == ANY_SERVICE)
                                  && (std::get<1>(m) == _i || std::get<1>(m) == ANY_INSTANCE)
                                  && (std::get<2>(m) == _m || std::get<2>(m) == ANY_METHOD)) {
                                  return true;
                              }
                          }
                          return false;
                      };
            } else {
                its_filter_func
                    = [_matches](service_t _s, instance_t _i, method_t) {
                          for (const auto &m : _matches) {
                              if ((std::get<0>(m) == _s || std::get<0>(m) == ANY_SERVICE)
                                  && (std::get<1>(m) == _i || std::get<1>(m) == ANY_INSTANCE)) {
                                  return true;
                              }
                          }
                          return false;
                      };
            }
        } else {
            if (has_method) {
                its_filter_func
                    = [_matches](service_t _s, instance_t, method_t _m) {
                          for (const auto &m : _matches) {
                              if ((std::get<0>(m) == _s || std::get<0>(m) == ANY_SERVICE)
                                  && (std::get<2>(m) == _m || std::get<2>(m) == ANY_METHOD)) {
                                  return true;
                              }
                          }
                          return false;
                      };
            } else {
                its_filter_func
                    = [_matches](service_t _s, instance_t, method_t) {
                          for (auto &m : _matches) {
                              if (std::get<0>(m) == _s || std::get<0>(m) == ANY_SERVICE) {
                                  return true;
                              }
                          }
                          return false;
                      };
            }
        }
    } else {
        if (has_instance) {
            if (has_method) {
                its_filter_func
                    = [_matches](service_t, instance_t _i, method_t _m) {
                          for (auto &m : _matches) {
                              if ((std::get<1>(m) == _i || std::get<1>(m) == ANY_INSTANCE)
                                  && (std::get<2>(m) == _m || std::get<2>(m) == ANY_METHOD)) {
                                  return true;
                              }
                          }
                          return false;
                      };
            } else {
                its_filter_func
                    = [_matches](service_t, instance_t _i, method_t) {
                          for (auto &m : _matches) {
                              if (std::get<1>(m) == _i || std::get<1>(m) == ANY_INSTANCE) {
                                  return true;
                              }
                          }
                          return false;
                      };
            }
        } else {
            if (has_method) {
                its_filter_func
                    = [_matches](service_t, instance_t, method_t _m) {
                          for (auto &m : _matches) {
                              if (std::get<2>(m) == _m || std::get<2>(m) == ANY_METHOD) {
                                  return true;
                              }
                          }
                          return false;
                      };
            } else {
                its_filter_func
                    = [](service_t, instance_t, method_t) {
                          return true;
                      };
            }
        }
    }

    return add_filter_intern(its_filter_func, _is_positive);
}

filter_id_t channel_impl::add_filter(
        const match_t &_from, const match_t &_to, bool _is_positive) {

    // Check usage of ANY_* which is forbidden here
    if (std::get<0>(_from) == ANY_SERVICE ||
      std::get<1>(_from) == ANY_INSTANCE ||
      std::get<2>(_from) == ANY_METHOD ||
      std::get<0>(_to) == ANY_SERVICE ||
      std::get<1>(_to) == ANY_INSTANCE ||
      std::get<2>(_to) == ANY_METHOD) {
      VSOMEIP_ERROR << "Trace filter configuration error: "
        "You must not use wildcards in range filters.";
      return FILTER_ID_ERROR;
    }

    std::function<bool (service_t, instance_t, method_t)> its_filter_func
            = [_from, _to](service_t _s, instance_t _i, method_t _m) {
        return (std::get<0>(_from) <= _s && _s <= std::get<0>(_to)
                && std::get<1>(_from) <= _i && _i <= std::get<1>(_to)
                && std::get<2>(_from) <= _m && _m <= std::get<2>(_to));
    };

    return add_filter_intern(its_filter_func, _is_positive);
}

void channel_impl::remove_filter(filter_id_t _id) {
    std::lock_guard<std::mutex> its_lock(mutex_);
    positive_.erase(_id);
    negative_.erase(_id);
}

filter_id_t channel_impl::add_filter_intern(const filter_func_t& _func, bool _is_positive) {
    filter_id_t its_id = current_filter_id_.fetch_add(1);

    std::lock_guard<std::mutex> its_lock(mutex_);
    if (_is_positive)
        positive_[its_id] = _func;
    else
        negative_[its_id] = _func;

    return its_id;
}

bool channel_impl::matches(
        service_t _service, instance_t _instance, method_t _method) {
    std::lock_guard<std::mutex> its_lock(mutex_);

    // If a negative filter matches --> drop!
    for (auto &its_filter : negative_) {
        if (its_filter.second(_service, _instance, _method)) {
            return false;
        }
    }

    // If no positive filter is defined --> forward!
    if (positive_.size() == 0)
        return true;

    // If a positive filter matches --> forward!
    for (auto &its_filter : positive_) {
        if (its_filter.second(_service, _instance, _method)) {
            return true;
        }
    }

    // drop!
    return false;
}

} // namespace trace
} // namespace vsomeip_v3
