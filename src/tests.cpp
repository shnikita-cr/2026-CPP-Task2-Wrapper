
#include <cstdio>
#include <exception>
#include <string>

#include "Wrapper.h"
#include "Engine.h"
#include "TextDocument.h"

auto main() -> int {
    try {
        TextDocument document("hello world. hello cpp!");

        Wrapper replace(&document,&TextDocument::replaceWords,{{"oldWord", std::string("hello")},{"newWord", std::string("hi")}});
        Wrapper find(&document,&TextDocument::findPhrase,{{"phrase", std::string("cpp")}});

        Engine engine;
        engine.register_command(&replace, "replace");
        engine.register_command(&find, "find");

        if (engine.execute("find", {}) != "found") {
            std::fprintf(stderr, "[FAIL] Find phrase\n");
            return 1;
        }

        if (engine.execute("replace", {}) != "hi world. hi cpp!") {
            std::fprintf(stderr, "[FAIL] Replace words\n");
            return 1;
        }

        std::printf("[PASS] All tests\n");
        return 0;
    }
    catch (const std::exception& error) {
        std::fprintf(stderr, "[FAIL] Unexpected exception: %s\n", error.what());
    }
    catch (...) {
        std::fprintf(stderr, "[FAIL] Unknown exception\n");
    }

    return 1;
}
