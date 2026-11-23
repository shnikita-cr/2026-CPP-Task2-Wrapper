#ifndef WRAPPER_H
#define WRAPPER_H

#include <functional>
#include <map>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>

using Arguments = std::map<std::string, std::string>;

class Wrapper {
public:
  template <class T, class Method>
  Wrapper(T *object, Method method, Arguments defaults)
      : call_([object, method](const Arguments &args) {
          if constexpr (std::is_invocable_v<Method, T &, const Arguments &>) {
            return std::invoke(method, *object, args);
          } else {
            return std::invoke(method, *object);
          }
        }),
        defaults_(std::move(defaults)) {
    if (object == nullptr) {
      throw std::invalid_argument("null object");
    }
  }

  [[nodiscard]] std::string execute(const Arguments &overrides = {}) const {
    Arguments args = defaults_;
    for (const auto &[name, value] : overrides) {
      args[name] = value;
    }
    return call_(args);
  }

private:
  std::function<std::string(const Arguments &)> call_;
  Arguments defaults_;
};

class Engine {
public:
  void register_command(Wrapper *wrapper, const std::string &name) {
    if (wrapper == nullptr) {
      throw std::invalid_argument("null wrapper");
    }
    commands_[name] = wrapper;
  }

  [[nodiscard]] std::string execute(const std::string &name,
                                    const Arguments &args = {}) const {
    return commands_.at(name)->execute(args);
  }

private:
  std::map<std::string, Wrapper *> commands_;
};

#endif
