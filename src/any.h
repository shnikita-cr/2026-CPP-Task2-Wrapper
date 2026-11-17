#ifndef ANY_H
#define ANY_H

#include <memory>
#include <string>
#include <typeinfo>
#include <stdexcept>

class Any {
public:
    Any() : m_type(&typeid(void)) {}

    Any(const Any& other)
        : m_type(other.m_type), m_holder(other.m_holder ? other.m_holder->clone() : nullptr) {
    }

    Any(Any&& other) noexcept = default;

    Any& operator=(const Any& other) {
        if (this != &other) {
            m_type = other.m_type;
            m_holder = other.m_holder ? std::unique_ptr<BaseHolder>(other.m_holder->clone()) : nullptr;
        }
        return *this;
    }

    Any& operator=(Any&& other) noexcept = default;
    ~Any() = default;

    template <typename T>
    Any(const T& value) : m_type(&typeid(T)), m_holder(new Holder<T>(value)) {}

    bool hasValue() const { return m_holder != nullptr; }
    const std::type_info& type() const { return *m_type; }

    template <typename T>
    T get() const {
        if (*m_type != typeid(T) || !m_holder) {
            throw std::runtime_error("Any: type mismatch");
        }
        return static_cast<Holder<T>*>(m_holder.get())->value;
    }

private:
    struct BaseHolder {
        virtual ~BaseHolder() = default;
        virtual BaseHolder* clone() const = 0;
    };

    template <typename T>
    struct Holder : BaseHolder {
        explicit Holder(const T& v) : value(v) {}
        BaseHolder* clone() const override { return new Holder<T>(value); }
        T value;
    };

    const std::type_info* m_type;
    std::unique_ptr<BaseHolder> m_holder;
};

#endif