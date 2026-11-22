#include "Engine.h"
#include "Factory.h"
#include "TextDocument.h"

#include <functional>
#include <iostream>
#include <string>

int main() {
    try {
        TextDocument doc("hello world. this is a test.");

        auto replaceMethod =
            [](TextDocument* obj,
                const Arguments& args) -> std::string {
                    return obj->replaceWord(args);
            };

        auto replaceWrapper = makeWrapper(
            &doc,
            replaceMethod,
            Arguments{ {"oldWord", "hello"}, {"newWord", "hi"} });

        Engine engine;
        engine.register_command(std::move(replaceWrapper), "replace");

        std::cout << engine.execute(
            "replace",
            { {"oldWord", "world"}, {"newWord", "earth"} })
            << std::endl;
        std::cout << "After replace: " << doc.getText() << std::endl;

    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}