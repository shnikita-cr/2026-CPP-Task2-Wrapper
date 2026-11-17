#pragma once

#include <any>
#include <string>
#include <type_traits>
#include <vector>

// Оборачивает значение неизвестного заранее копируемого типа.
class ArgumentValue
{
public:
    template<typename T>
    ArgumentValue(const T& value)
    {
        // std::decay_t приводит тип к обычному типу значения. Он убирает ссылки, const и преобразует массив в указатель.
        using ValueType = std::decay_t<T>;

        // Строковые литералы и указатели на символы храним как самостоятельную std::string.
        // std::is_same_v - сравнивает два типа во время компиляции.
        if constexpr (std::is_same_v<ValueType, const char*> || std::is_same_v<ValueType, char*>)
        {
            _value = std::string(value);
        }
        else
        {
            _value = value;
        }
    }

    // Тип значения должен совпадать с типом параметра без ссылки и const.
    // Метод as<T>() достаёт значение из ArgumentValue.
    template<typename T>
    [[nodiscard]] std::decay_t<T> as() const
    {
        return std::any_cast<std::decay_t<T>>(_value);
    }

private:
    std::any _value; // Реальное значение вместе с информацией о его типе.
};

// Хранит имя параметра и связанное с ним значение.
struct NamedArgument
{
    std::string name;
    ArgumentValue value;
};

// Список сохраняет порядок параметров метода.
using ArgumentList = std::vector<NamedArgument>;
