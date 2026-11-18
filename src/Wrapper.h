#include <string>
#include <array>
#include <map>
#include "Any.h"

class Wrapper
{
    struct Concept
    {
        virtual ~Concept() = default;
        [[nodiscard]] virtual Any execute(const std::map<std::string, Any>&) const = 0;
    };

    template<typename Subject, typename Method, typename Ret, typename... Args>
    struct Model : Concept
    {
        Subject* subject;
        Method method;
        std::map<std::string, Any> defaults;
        std::array<std::string, sizeof...(Args)> names;

        Model(Subject* sub, Method met, std::initializer_list<std::pair<std::string, Any>> dflt) : subject(sub), method(met)
        {
            if (dflt.size() != sizeof...(Args))
            {
                throw std::invalid_argument("Wrapper: argument count mismatch with default dictionary size");
            }
            std::size_t iter = 0;
            for (const auto& pair : dflt)
            {
                names[iter++] = pair.first;
                defaults[pair.first] = pair.second;
            }
        }

        [[nodiscard]] Any execute(const std::map<std::string, Any>& overrides) const override
        {
            std::map<std::string, Any> merged = defaults;
            for (const auto& [k, v] : overrides)
            {
                merged[k] = v;
            }
            return invoke(merged, std::index_sequence_for<Args...>{});
        }

    private:
        template<std::size_t... Is>
        Any invoke(const std::map<std::string, Any>& amap, std::index_sequence<Is...> /*j*/) const
        {
            if constexpr (std::is_void_v<Ret>)
            {
                (subject->*method)(amap.at(names[Is]).template get<Args>()...);
                return Any{};
            }
            else
            {
                return Any((subject->*method)(amap.at(names[Is]).template get<Args>()...));
            }
        }
    };

    std::unique_ptr<Concept> impl_;

public:
    template<typename Subject, typename Ret, typename... Args>
    Wrapper(Subject* subject, Ret (Subject::*method)(Args...), std::initializer_list<std::pair<std::string, Any>> defaults) : impl_(std::make_unique<Model<Subject, Ret (Subject::*)(Args...), Ret, Args...>>(subject, method, defaults))
    {}

    template<typename Subject, typename Ret, typename... Args>
    Wrapper(Subject* subject, Ret (Subject::*method)(Args...) const, std::initializer_list<std::pair<std::string, Any>> defaults) : impl_(std::make_unique<Model<Subject, Ret (Subject::*)(Args...) const, Ret, Args...>>(subject, method, defaults))
    {}

    [[nodiscard]] Any execute(const std::map<std::string, Any> &overrides = {}) const
    {
        return impl_->execute(overrides);
    }
};