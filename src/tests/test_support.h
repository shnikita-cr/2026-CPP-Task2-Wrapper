#ifndef WRAPPER_TEST_SUPPORT_H
#define WRAPPER_TEST_SUPPORT_H

#include <exception>
#include <initializer_list>
#include <iostream>
#include <stdexcept>
#include <string>
#include <utility>

namespace test_support
{

inline void require(bool condition, const std::string& message)
{
    if (!condition)
    {
        throw std::runtime_error(message);
    }
}

template <typename Exception, typename Callable> void require_throws(Callable&& callable)
{
    try
    {
        std::forward<Callable>(callable)();
    }
    catch (const Exception&)
    {
        return;
    }

    throw std::runtime_error("Expected exception was not thrown");
}

struct TestCase
{
    const char* name;
    void        (*function)();
};

inline int run(std::initializer_list<TestCase> tests)
{
    int failures = 0;
    for (const auto& test : tests)
    {
        try
        {
            test.function();
            std::cout << "PASS: " << test.name << '\n';
        }
        catch (const std::exception& error)
        {
            ++failures;
            std::cerr << "FAIL: " << test.name << ": " << error.what() << '\n';
        }
        catch (...)
        {
            ++failures;
            std::cerr << "FAIL: " << test.name << ": unknown exception\n";
        }
    }

    std::cout << tests.size() << " tests, " << failures << " failures\n";
    return failures == 0 ? 0 : 1;
}

} // namespace test_support

#endif // WRAPPER_TEST_SUPPORT_H
