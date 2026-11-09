//
//  any_map.h
//  Wrapper
//
//  Created by Богдан on 08.11.2026.
//

#ifndef ANY_MAP_H
#define ANY_MAP_H

#include <any>
#include <map>
#include <stdexcept>
#include <string>

using AnyMap = std::map<std::string, std::any>;

template <typename T>
T get_arg(const AnyMap& args, const std::string& key) {
    auto it = args.find(key);
    if (it == args.end()) {
        throw std::runtime_error("Missing argument: " + key);
    }
    try {
        return std::any_cast<T>(it->second);
    } catch (const std::bad_any_cast&) {
        throw std::runtime_error("Bad type for argument: " + key);
    }
}

template <typename T>
T get_arg_or(const AnyMap& args, const std::string& key, const T& default_value) {
    auto it = args.find(key);
    if (it == args.end()) {
        return default_value;
    }
    try {
        return std::any_cast<T>(it->second);
    } catch (const std::bad_any_cast&) {
        throw std::runtime_error("Bad type for argument: " + key);
    }
}

inline AnyMap merge_args(const AnyMap& defaults, const AnyMap& overrides) {
    AnyMap result = defaults;
    for (const auto& pair : overrides) {
        result[pair.first] = pair.second;
    }
    return result;
}

#endif  // ANY_MAP_H
