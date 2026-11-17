#ifndef WRAPPER_H
#define WRAPPER_H

#include "arg_value.h"
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <tuple>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <vector>

template <typename T>
struct MemberFunctionTraits;

template <typename Subject, typename ReturnType, typename... Args>
struct MemberFunctionTraits<ReturnType (Subject::*)(Args...)> {
    using ClassType = Subject;
    using Return = ReturnType;
    using ArgsTuple = std::tuple<Args...>;
    static constexpr std::size_t Arity = sizeof...(Args);
    static constexpr bool IsConst = false;
};

template <typename Subject, typename ReturnType, typename... Args>
struct MemberFunctionTraits<ReturnType (Subject::*)(Args...) const> {
    using ClassType = Subject;
    using Return = ReturnType;
    using ArgsTuple = std::tuple<Args...>;
    static constexpr std::size_t Arity = sizeof...(Args);
    static constexpr bool IsConst = true;
};

template <typename Subject, typename ReturnType, typename... Args>
struct MemberFunctionTraits<ReturnType (Subject::*)(Args...) noexcept> {
    using ClassType = Subject;
    using Return = ReturnType;
    using ArgsTuple = std::tuple<Args...>;
    static constexpr std::size_t Arity = sizeof...(Args);
    static constexpr bool IsConst = false;
};

template <typename Subject, typename ReturnType, typename... Args>
struct MemberFunctionTraits<ReturnType (Subject::*)(Args...) const noexcept> {
    using ClassType = Subject;
    using Return = ReturnType;
    using ArgsTuple = std::tuple<Args...>;
    static constexpr std::size_t Arity = sizeof...(Args);
    static constexpr bool IsConst = true;
};

class Wrapper {
private:
    struct IInvoker {
        virtual ~IInvoker() = default;
        virtual std::string invoke(const ArgsMap& args) = 0;
        virtual std::unique_ptr<IInvoker> clone() const = 0;
    };

    template <typename Subject, typename MethodPtr, typename ReturnType, typename... Args>
    class MethodInvoker : public IInvoker {
    private:
        Subject* subj_;
        MethodPtr method_;
        std::vector<std::string> param_names_;
        std::unordered_map<std::string, ArgValue> defaults_;

        template <std::size_t Index>
        auto get_arg(const ArgsMap& args) {
            using RawType = std::tuple_element_t<Index, std::tuple<Args...>>;
            using CleanType = std::decay_t<RawType>;
            const std::string& name = param_names_.at(Index);
            const ArgValue* val = args.find(name);
            if (val == nullptr) {
                auto found_default = defaults_.find(name);
                if (found_default != defaults_.end()) {
                    val = &found_default->second;
                }
            }
            if (val == nullptr) {
                throw std::invalid_argument("Missing required argument: " + name);
            }
            return val->template as<CleanType>();
        }

        template <std::size_t... Is>
        std::string invoke_impl(const ArgsMap& args, std::index_sequence<Is...>) {
            if constexpr (std::is_void_v<ReturnType>) {
                (subj_->*method_)(get_arg<Is>(args)...);
                return "";
            } else {
                auto res = (subj_->*method_)(get_arg<Is>(args)...);
                using CleanReturn = std::decay_t<ReturnType>;
                if constexpr (std::is_same_v<CleanReturn, std::string>) {
                    return res;
                } else if constexpr (std::is_same_v<CleanReturn, const char*>) {
                    return res != nullptr ? std::string(res) : std::string();
                } else if constexpr (std::is_same_v<CleanReturn, bool>) {
                    return res ? "true" : "false";
                } else if constexpr (std::is_arithmetic_v<CleanReturn>) {
                    return std::to_string(res);
                } else {
                    std::ostringstream oss;
                    oss << res;
                    return oss.str();
                }
            }
        }

    public:
        MethodInvoker(Subject* subj, MethodPtr method, const ArgsMap& defaults)
            : subj_(subj), method_(method) {
            if (defaults.size() != sizeof...(Args)) {
                throw std::invalid_argument("Defaults count does not match function parameter count");
            }
            param_names_.reserve(defaults.size());
            for (const auto& entry : defaults.entries()) {
                param_names_.push_back(entry.first);
                defaults_[entry.first] = entry.second;
            }
        }

        std::unique_ptr<IInvoker> clone() const override {
            return std::make_unique<MethodInvoker>(*this);
        }

        std::string invoke(const ArgsMap& args) override {
            return invoke_impl(args, std::index_sequence_for<Args...>{});
        }
    };

    std::unique_ptr<IInvoker> invoker_;

public:
    Wrapper() = default;

    template <typename Subject, typename MethodPtr,
              typename Traits = MemberFunctionTraits<MethodPtr>,
              typename = std::enable_if_t<
                  std::is_base_of_v<typename Traits::ClassType, Subject> ||
                  std::is_same_v<typename Traits::ClassType, Subject>>>
    Wrapper(Subject* subj, MethodPtr method, const ArgsMap& defaults = {}) {
        if (subj == nullptr) {
            throw std::invalid_argument("Subject pointer cannot be null");
        }
        using Return = typename Traits::Return;
        invoker_ = unpack_and_create<Subject, MethodPtr, Return>(
            subj, method, defaults, static_cast<typename Traits::ArgsTuple*>(nullptr));
    }

    template <typename Subject, typename MethodPtr,
              typename Traits = MemberFunctionTraits<MethodPtr>,
              typename = std::enable_if_t<
                  (std::is_base_of_v<typename Traits::ClassType, Subject> ||
                   std::is_same_v<typename Traits::ClassType, Subject>) &&
                  Traits::IsConst>>
    Wrapper(const Subject* subj, MethodPtr method, const ArgsMap& defaults = {}) {
        if (subj == nullptr) {
            throw std::invalid_argument("Subject pointer cannot be null");
        }
        using Return = typename Traits::Return;
        invoker_ = unpack_and_create<const Subject, MethodPtr, Return>(
            subj, method, defaults, static_cast<typename Traits::ArgsTuple*>(nullptr));
    }

    Wrapper(const Wrapper& other)
        : invoker_(other.invoker_ ? other.invoker_->clone() : nullptr) {}

    Wrapper& operator=(const Wrapper& other) {
        if (this != &other) {
            invoker_ = other.invoker_ ? other.invoker_->clone() : nullptr;
        }
        return *this;
    }

    Wrapper(Wrapper&&) noexcept = default;
    Wrapper& operator=(Wrapper&&) noexcept = default;
    ~Wrapper() = default;

    bool is_valid() const noexcept {
        return invoker_ != nullptr;
    }

    std::string execute(const ArgsMap& args = {}) const {
        if (!invoker_) {
            throw std::runtime_error("Attempted to execute an uninitialized Wrapper");
        }
        return invoker_->invoke(args);
    }

    template <typename T>
    T execute(const ArgsMap& args = {}) const {
        std::string res = execute(args);
        return ArgValue(res).template as<T>();
    }

private:
    template <typename Subj, typename MethodPtr, typename ReturnType, typename... Args>
    static std::unique_ptr<IInvoker> unpack_and_create(
        Subj* subj, MethodPtr method, const ArgsMap& defaults, std::tuple<Args...>*) {
        return std::make_unique<MethodInvoker<Subj, MethodPtr, ReturnType, Args...>>(
            subj, method, defaults);
    }
};

#endif // WRAPPER_H
