#include "value.h"

std::string Value::describe() const {
    if (!m_data.has_value()) {
        return "<empty>";
    }
    if (const std::optional<std::string> text = try_as<std::string>(); text.has_value()) {
        return "'" + *text + "'";
    }
    return std::string("of type <") + m_data.type().name() + ">";
}