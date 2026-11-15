#ifndef WRAPPER_H
#define WRAPPER_H

#include <functional>
#include <map>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

class AnyComand {
 public:
  AnyComand() = default;
  virtual ~AnyComand() = default;

  AnyComand(const AnyComand&) = default;
  AnyComand& operator=(const AnyComand&) = default;
  AnyComand(AnyComand&&) noexcept = default;
  AnyComand& operator=(AnyComand&&) noexcept = default;
};

template <typename ArgType = std::string, typename ReturnType = std::string>
class ComandBase : public AnyComand {
 public:
  using ArgsMap = std::map<std::string, ArgType>;

  virtual ReturnType execute(const ArgsMap& override_args) const = 0;
};

template <typename ArgType = std::string, typename ReturnType = std::string>
class Wrapper : public ComandBase<ArgType, ReturnType> {
 public:
  using ArgsList = std::vector<std::pair<std::string, ArgType>>;
  using ArgsMap = std::map<std::string, ArgType>;

  template <typename Subject, typename... MethodArgs>
  Wrapper(Subject* subj, ReturnType (Subject::*method)(MethodArgs...),
          ArgsList default_args)
      : default_args_(std::move(default_args)) {
    if (subj == nullptr) {
      throw std::invalid_argument("Subjct ptr cant 0");
    }
    if (default_args_.size() != sizeof...(MethodArgs)) {
      throw std::invalid_argument("Args count invalid");
    }

    invoker_ = [subj,
                method](const std::vector<ArgType>& values) -> ReturnType {
      return call_method(subj, method, values,
                         std::index_sequence_for<MethodArgs...>{});
    };
  }

  template <typename Subject, typename... MethodArgs>
  Wrapper(const Subject* subj,
          ReturnType (Subject::*method)(MethodArgs...) const,
          ArgsList default_args)
      : default_args_(std::move(default_args)) {
    if (subj == nullptr) {
      throw std::invalid_argument("Subjct ptr cant 0");
    }
    if (default_args_.size() != sizeof...(MethodArgs)) {
      throw std::invalid_argument("Args count invalid");
    }

    invoker_ = [subj,
                method](const std::vector<ArgType>& values) -> ReturnType {
      return call_const_method(subj, method, values,
                               std::index_sequence_for<MethodArgs...>{});
    };
  }

  ReturnType execute(const ArgsMap& override_args) const override {
    std::vector<ArgType> final_args;
    final_args.reserve(default_args_.size());

    for (const auto& pair_item : default_args_) {
      auto found = override_args.find(pair_item.first);
      if (found != override_args.end()) {
        final_args.push_back(found->second);
      } else {
        final_args.push_back(pair_item.second);
      }
    }
    return invoker_(final_args);
  }

 private:
  ArgsList default_args_;
  std::function<ReturnType(const std::vector<ArgType>&)> invoker_;

  template <typename Subject, typename... MethodArgs, std::size_t... Indices>
  static ReturnType call_method(Subject* subj,
                                ReturnType (Subject::*method)(MethodArgs...),
                                const std::vector<ArgType>& values,
                                std::index_sequence<Indices...>) {
    return (subj->*method)(static_cast<MethodArgs>(values[Indices])...);
  }

  template <typename Subject, typename... MethodArgs, std::size_t... Indices>
  static ReturnType call_const_method(
      const Subject* subj, ReturnType (Subject::*method)(MethodArgs...) const,
      const std::vector<ArgType>& values, std::index_sequence<Indices...>) {
    return (subj->*method)(static_cast<MethodArgs>(values[Indices])...);
  }
};

template <typename Subject, typename ReturnType, typename FirstArg,
          typename... RestArgs>
Wrapper(Subject*, ReturnType (Subject::*)(FirstArg, RestArgs...),
        std::vector<std::pair<std::string, std::decay_t<FirstArg>>>)
    -> Wrapper<std::decay_t<FirstArg>, ReturnType>;

template <typename Subject, typename ReturnType, typename FirstArg,
          typename... RestArgs>
Wrapper(const Subject*, ReturnType (Subject::*)(FirstArg, RestArgs...) const,
        std::vector<std::pair<std::string, std::decay_t<FirstArg>>>)
    -> Wrapper<std::decay_t<FirstArg>, ReturnType>;

template <typename... Args>
decltype(auto) make_wrapper(Args&&... args) {
  return Wrapper(std::forward<Args>(args)...);
}

#endif