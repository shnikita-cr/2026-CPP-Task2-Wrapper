#include "ArgumentMap.h"

#include <algorithm>
#include <iterator>

ArgumentMap::ArgumentMap(std::initializer_list<Entry> entries) : entries_(entries) {}

std::size_t ArgumentMap::size() const noexcept {
    return entries_.size();
}

const std::vector<ArgumentMap::Entry>& ArgumentMap::entries() const noexcept {
    return entries_;
}

std::vector<Value> ArgumentMap::values() const {
    std::vector<Value> result;
    result.reserve(entries_.size());
    std::transform(entries_.begin(), entries_.end(), std::back_inserter(result),
                   [](const Entry& entry) { return entry.second; });
    return result;
}

bool ArgumentMap::replace(const std::string& name, const Value& value) {
    const auto found = std::find_if(entries_.begin(), entries_.end(),
                                    [&name](const Entry& entry) { return entry.first == name; });
    if (found == entries_.end()) {
        return false;
    }
    found->second = value;
    return true;
}
