#pragma once

#include "wrapper_engine/types.hpp"

#include <algorithm>
#include <cstddef>
#include <functional>
#include <initializer_list>
#include <stdexcept>
#include <string>
#include <unordered_set>
#include <utility>

// Хранит объект, его метод и именованные аргументы по умолчанию.
class Wrapper
{
public:
    template<typename ObjectType, typename... ParameterTypes>
    Wrapper(ObjectType* object, std::string (ObjectType::*method)(ParameterTypes...), std::initializer_list<NamedArgument> defaultArguments)
    {
        // Wrapper не может вызвать метод без существующего объекта.
        if (object == nullptr)
        {
            throw std::invalid_argument("Wrapper cannot store a null object pointer.");
        }

        // Каждому параметру метода должно соответствовать одно имя и значение.
        if (defaultArguments.size() != sizeof...(ParameterTypes))
        {
            throw std::invalid_argument("The number of default arguments does not match the method signature.");
        }

        // Список хранит имена и значения в порядке параметров метода.
        for (const auto& argument : defaultArguments)
        {
            // Пустое имя невозможно использовать при последующем вызове.
            if (argument.name.empty())
            {
                throw std::invalid_argument("An argument name cannot be empty.");
            }

            // Имя должно однозначно указывать на один параметр.
            if (find_argument(_defaultArguments, argument.name) != _defaultArguments.end())
            {
                throw std::invalid_argument("A default argument name is repeated: " + argument.name);
            }

            _defaultArguments.push_back(argument);
        }

        // Лямбда запоминает объект и метод. Порядок уже задан списком.
        _executor = [object, method](const ArgumentList& arguments)
        {
            // По одному индексу на каждый параметр метода.
            return invoke_member(object, method, arguments, std::index_sequence_for<ParameterTypes...>{});
        };
    }

    // Заменяет переданные значения и выполняет сохранённый метод.
    [[nodiscard]] std::string execute(const ArgumentList& newArguments = {}) const
    {
        // Каждый вызов начинает с неизменных аргументов по умолчанию.
        ArgumentList arguments = _defaultArguments;
        // std::unordered_set - это множество уникальных элементов.
        std::unordered_set<std::string> changedNames;

        for (const auto& argument : newArguments)
        {
            // Одно имя нельзя дважды передать в пределах одного вызова.
            if (!changedNames.emplace(argument.name).second)
            {
                throw std::invalid_argument("The argument is specified more than once: " + argument.name);
            }

            // Новое значение разрешено только для известного параметра.
            const auto iterator = find_argument(arguments, argument.name);
            if (iterator == arguments.end())
            {
                throw std::invalid_argument("Unknown argument: " + argument.name);
            }

            // Остальные значения остаются равными значениям по умолчанию.
            iterator->value = argument.value;
        }

        // Типизированный вызов скрыт внутри сохранённой функции.
        return _executor(arguments);
    }

private:
    static ArgumentList::iterator find_argument(ArgumentList& arguments, const std::string& name)
    {
        // Поиск возвращает позицию аргумента или конец списка.
        return std::find_if(arguments.begin(), arguments.end(), [&name](const NamedArgument& argument)
        {
            return argument.name == name;
        });
    }

    template<typename ObjectType, typename... ParameterTypes, std::size_t... Indexes>
    static std::string invoke_member(ObjectType* object, std::string (ObjectType::*method)(ParameterTypes...), const ArgumentList& arguments, std::index_sequence<Indexes...> /*indexes*/)
    {
        // Извлекаем значения в порядке параметров и сразу вызываем метод.
        return (object->*method)(arguments[Indexes].value.template as<ParameterTypes>()...);
    }

    ArgumentList _defaultArguments; // Исходные значения для каждого нового вызова.
    std::function<std::string(const ArgumentList&)> _executor; // Единая форма вызова.
};
