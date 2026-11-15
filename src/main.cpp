#include <iostream>
#include "placeholder.h"
#include "Wrapper.h"
#include "Engine.h"
#include "Test.h"
#include "Formatter.h"
#include <memory>


int main(){
    try {
        constexpr int kDefaultIntValue = 5;

        std::cout << "Hello Wrapper!\n";
        std::cout << "Hello Engine!\n";
        Engine engine;

        InitialArchitecturePlaceholder::projectInitialized();

        Test subj;

        auto wrapper = make_wrapper(&subj, &Test::execute, {
            {"string", std::string("-")}, 
            {"int", 0}, 
            {"bool", false}
        });
        engine.register_command(std::move(wrapper), "command_name"); 
        auto result = engine.execute("command_name", {{"string", std::string("hello")}, {"int", kDefaultIntValue}});
        std::cout << std::any_cast<int>(result) << "\n";

        Formatter form;

        auto wrapper1 = make_wrapper(&form, &Formatter::isStringInText, {
            {"text", std::string("Default text")},
            {"request", std::string(" ")}});

        engine.register_command(std::move(wrapper1), "find");
        auto result1 = engine.execute("find", {
            {std::string("request"), std::string("me")},
            {std::string("text"), std::string("Someone find me in this text")}
            });
        std::cout << std::any_cast<bool>(result1) << "\n";

        auto wrapper2 = make_wrapper(&form, &Formatter::replaceWords, {
            {"text", std::string("Default text")},
            {"from", std::string(".")},
            {"to", std::string(". ")}
            });

        engine.register_command(std::move(wrapper2), "replace");
        auto result2 = engine.execute("replace", {
            {std::string("text"), std::string("Someone find me in this text")},
            {std::string("to"), std::string("us")},
            {std::string("from"), std::string("me")}
            });
        std::cout << std::any_cast<std::string>(result2) << "\n";

        auto wrapper3 = make_wrapper(&form, &Formatter::firstCapitalize, {
            {"text", std::string("default text")}
            });

        engine.register_command(std::move(wrapper3), "first_capitalize");
        auto result3 = engine.execute("first_capitalize", {
            {std::string("text"), std::string("please capitalize it. thanks. if not, then enumerate. appreciate it")}
            });
        std::cout << std::any_cast<std::string>(result3) << "\n";

        auto wrapper4 = make_wrapper(&form, &Formatter::eachCapitalize, {
            {"text", std::string("default text")}
            });

        engine.register_command(std::move(wrapper4), "each_capitalize");
        auto result4 = engine.execute("each_capitalize", {
            {std::string("text"), std::string("please capitalize it. thanks. if not, then enumerate. appreciate it")}
            });
        std::cout << std::any_cast<std::string>(result4) << "\n";

        auto wrapper5 = make_wrapper(&form, &Formatter::enumerateSentences, {
            {"text", std::string("Default text. Example")}
            });

        engine.register_command(std::move(wrapper5), "enumerate");
        auto result5 = engine.execute("enumerate", {
            {std::string("text"), std::string("please capitalize it. thanks. if not, then enumerate. appreciate it")}
            });
        std::cout << std::any_cast<std::string>(result5) << "\n";
        return 0; 
    } catch (const std::exception& eee) {
        std::cerr << "Ошибка: " << eee.what() << "\n";
        return 1;
    }
}