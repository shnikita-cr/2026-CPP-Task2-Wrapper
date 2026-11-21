
#pragma once
#include <any>
#include <cstddef>
#include <functional>
#include <map>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

// Vector instead of map to preserve arguments order
using Arguments_t = std::vector<std::pair<std::string, std::any>>;

template <typename Return_t>
class Wrapper {
public:
    // For non-const method
    template <typename Class_t, typename... Params_ts>
    Wrapper(
        Class_t* object,
        Return_t (Class_t::*method)(Params_ts...),
        Arguments_t defaults) 
    {
        bind<Class_t, Return_t (Class_t::*)(Params_ts...), Params_ts...>(
            object, method, std::move(defaults));
    }

    // For const method
    template <typename Class_t, typename... Params_ts>
    Wrapper(
        Class_t* object,
        Return_t (Class_t::*method)(Params_ts...) const,
        Arguments_t defaults)
    {
        bind<Class_t, Return_t (Class_t::*)(Params_ts...) const, Params_ts...>(
            object, method, std::move(defaults)
        );
    }

    // Execute wrapped method
    [[nodiscard]] Return_t execute(const Arguments_t& new_args = {}) const {
        if (!saved_method) {
            throw std::logic_error("Wrapper has no bound method");
        }
        // Execute saved function with new args
        return saved_method(new_args);
    }

private:
    // Function that stores the method call
    std::function<Return_t(const Arguments_t&)> saved_method;

    // Create saved_method lambda function
    template <typename Class_t, typename Method_t, typename... Params_ts>
    void bind(Class_t* object, Method_t method, Arguments_t defaults) {
        if (object == nullptr) {
            throw std::invalid_argument("Null object");
        }

        // Check numbers of arguments
        if (defaults.size() != sizeof...(Params_ts)) {
            throw std::invalid_argument("Wrong number of arguments");
        }


        // Checking for argument names
        for (std::size_t i = 0; i < defaults.size(); ++i) {

            if (defaults[i].first.empty()) {
                throw std::invalid_argument("Empty argument name");
            }

            // Check for duplicate names
            for (std::size_t j = i + 1; j < defaults.size(); ++j) {
                if (defaults[i].first == defaults[j].first) {
                    throw std::invalid_argument("Duplicate argument name");
                }
            }

            // Check that arg is not empty
            if (!defaults[i].second.has_value()) {
                throw std::invalid_argument("Empty argument value");
            }
        }        

        // Container for method + default args
        saved_method = [
            object,
            method,
            defaults = std::move(defaults)
        ](const Arguments_t& new_args) -> Return_t {

            // Copy default arguments
            Arguments_t default_args = defaults;

            // Iterate through all new arguments
            for (const auto& [name, value] : new_args) {

                // Checks
                if (!value.has_value()) {
                    throw std::invalid_argument("Empty argument value");
                }

                //Search for new_arg in default_args
                auto iter = default_args.begin();
                while (iter != default_args.end() && iter->first != name) {
                    ++iter;
                }
                
                if (iter == default_args.end()) {
                    throw std::invalid_argument("Unknown argument: " + name);
                }

                // Replace arg in default_args
                iter->second = value;
            }

            // Call the method with new args
            // index_sequence_for - creates 0, 1, 2, ... for number
            // of Params_ts...
            return invoke<Class_t, Method_t, Params_ts...>(
                object,
                method,
                default_args,
                std::index_sequence_for<Params_ts...>{}
            );
        };
    }

    // Invoke the method with arguments of correct types
    template <
        typename Class_t,
        typename Method_t,
        typename... Params_ts,
        std::size_t... I
    >
    static Return_t invoke(
        Class_t* object,
        Method_t method,
        Arguments_t& values,
        std::index_sequence<I...> /*indices*/
    ) {
        // Directly calling the method of object with args of correct types
        return std::invoke(
            method,
            object,
            get_argument<Params_ts>(values[I].second)...
        );
        // get_argument<Params_ts>(values[I].second)... - expands
        // to get_argument<Params_t1>(values[0].second),
        //    get_argument<Params_t2>(values[1].second),
        //    ...
    }

    // Method for extracting value (of std::any type) with
    // clean Param_t type (without cosnt and &), preserving reference
    template <typename Param_t>
    static decltype(auto) get_argument(std::any& value) {
        // decltype(auto) - to save & when returning
        using CleanParam_t = std::remove_cv_t<std::remove_reference_t<Param_t>>;

        if constexpr (std::is_lvalue_reference_v<Param_t>) {
            if constexpr (std::is_const_v<std::remove_reference_t<Param_t>>
            ) {
                return std::any_cast<const CleanParam_t&>(value);
            } else {
                return std::any_cast<CleanParam_t&>(value);
            }

        } else {
            // Return copy by value
            return std::any_cast<CleanParam_t>(value);
        }
    }
};

// Class template argument deduction (to use Wrapper w() instead of Wrapper<Retyrn_t> w())
template <typename Class_t, typename Return_t, typename... Params_ts>
Wrapper(Class_t*, Return_t (Class_t::*)(Params_ts...), Arguments_t)
    -> Wrapper<Return_t>;

// For const methods
template <typename Class_t, typename Return_t, typename... Params_ts>
Wrapper(Class_t*, Return_t (Class_t::*)(Params_ts...) const, Arguments_t)
    -> Wrapper<Return_t>;
