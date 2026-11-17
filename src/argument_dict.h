#ifndef ARGUMENT_DICT_H
#define ARGUMENT_DICT_H

#include <map>
#include <string>
#include <initializer_list>
#include "any.h"

using ArgumentDict = std::map<std::string, Any>;

inline ArgumentDict mergeArgs(const ArgumentDict& defaults, const ArgumentDict& overrides) {
    ArgumentDict result = defaults;
    for (const auto& kv : overrides) {
        result[kv.first] = kv.second;
    }
    return result;
}

#endif