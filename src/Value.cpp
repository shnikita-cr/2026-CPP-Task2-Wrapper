#include "Value.h"

std::string Value::toString() const {
    if (printer_ == nullptr) {
        return {};
    }
    return printer_(data_);
}

void Value::throwBadConversion(const char* requested) const {
    throw std::invalid_argument(std::string("Value: cannot convert ") + data_.type().name() + " to " + requested);
}
