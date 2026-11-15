#ifndef WRAPPER_H
#define WRAPPER_H

#include <cstddef>
#include <functional>
#include <map>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

class Wrapper {
public:
    using ArgMap = std::map<std::string, std::string>;
    using ArgList = std::vector<std::pair<std::string, std::string>>;

    template <typename Subject, typename... Args>
    Wrapper(Subject* subj,
            std::string (Subject::*method)(Args...),
            const ArgList& default_args = {}) {
        init_arguments(subj, method != nullptr, sizeof...(Args), default_args);
        caller_ = [subj, method](const std::vector<std::string>& args) -> std::string {
            return call_helper(subj, method, args, std::index_sequence_for<Args...>{});
        };
    }

    template <typename Subject, typename... Args>
    Wrapper(const Subject* subj,
            std::string (Subject::*method)(Args...) const,
            const ArgList& default_args = {}) {
        init_arguments(subj, method != nullptr, sizeof...(Args), default_args);
        caller_ = [subj, method](const std::vector<std::string>& args) -> std::string {
            return call_const_helper(subj, method, args, std::index_sequence_for<Args...>{});
        };
    }

    ~Wrapper() noexcept = default;
    Wrapper(const Wrapper&) = default;
    Wrapper& operator=(const Wrapper&) = default;
    Wrapper(Wrapper&&) noexcept = default;
    Wrapper& operator=(Wrapper&&) noexcept = default;

    [[nodiscard]] std::string execute(const ArgMap& override_args = {}) const {
        for (const auto& pair : override_args) {
            if (default_args_.find(pair.first) == default_args_.end()) {
                throw std::invalid_argument("Unknown argument name: " + pair.first);
            }
        }

        std::vector<std::string> final_args;
        final_args.reserve(arg_names_.size());

        for (const auto& name : arg_names_) {
            auto iter = override_args.find(name);
            if (iter != override_args.end()) {
                final_args.push_back(iter->second);
            } else {
                final_args.push_back(default_args_.at(name));
            }
        }

        return caller_(final_args);
    }

private:
    void init_arguments(const void* subj,
                        bool has_method,
                        std::size_t expected_count,
                        const ArgList& default_args) {
        if (subj == nullptr || !has_method) {
            throw std::invalid_argument("Subject pointer or method pointer is null");
        }
        if (default_args.size() != expected_count) {
            throw std::invalid_argument("Argument count mismatch");
        }

        for (const auto& pair : default_args) {
            if (default_args_.find(pair.first) != default_args_.end()) {
                throw std::invalid_argument("Duplicate argument name: " + pair.first);
            }
            arg_names_.push_back(pair.first);
            default_args_[pair.first] = pair.second;
        }
    }

    template <typename Subject, typename... Args, std::size_t... Is>
    static std::string call_helper(Subject* subj,
                                   std::string (Subject::*method)(Args...),
                                   const std::vector<std::string>& args,
                                   std::index_sequence<Is...>) {
        return (subj->*method)(args[Is]...);
    }

    template <typename Subject, typename... Args, std::size_t... Is>
    static std::string call_const_helper(const Subject* subj,
                                         std::string (Subject::*method)(Args...) const,
                                         const std::vector<std::string>& args,
                                         std::index_sequence<Is...>) {
        return (subj->*method)(args[Is]...);
    }

    std::vector<std::string> arg_names_;
    ArgMap default_args_;
    std::function<std::string(const std::vector<std::string>&)> caller_;
};

#endif 
