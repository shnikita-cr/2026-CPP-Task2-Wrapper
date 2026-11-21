#include <iostream>
#include <clocale>
#include <string>

#ifdef _WIN32
#include <windows.h>
#endif

#include "wrapper.h"
#include "engine.h"
#include "text_processor.h"

int main()
{
    setlocale(LC_ALL, "");

#ifdef _WIN32
    SetConsoleOutputCP(1251);
    SetConsoleCP(1251);
#endif

    const std::string base_text =
        "\xE7\xE4\xF0\xE0\xE2\xF1\xF2\xE2\xF3\xE9 "
        "\xEC\xE8\xF0. "
        "\xFD\xF2\xEE \xEF\xF0\xEE\xE2\xE5\xF0\xEA\xE0 "
        "\xEF\xF0\xEE\xE3\xF0\xE0\xEC\xEC\xFB! "
        "\xE4\xEE\xE1\xF0\xEE \xEF\xEE\xE6\xE0\xEB\xEE\xE2\xE0\xF2\xFC "
        "\xE2 c++.";

    std::cout << "1. " << "\xC7\xE0\xEC\xE5\xED\xE0 \xF1\xEB\xEE\xE2:" << '\n';

    {
        TextProcessor doc(
            "\xEF\xF0\xE8\xE2\xE5\xF2 \xEC\xE8\xF0. "
            "\xFD\xF2\xEE \xEF\xF0\xEE\xE2\xE5\xF0\xEA\xE0 "
            "\xEF\xF0\xEE\xE3\xF0\xE0\xEC\xEC\xFB! "
            "\xE4\xEE\xE1\xF0\xEE \xEF\xEE\xE6\xE0\xEB\xEE\xE2\xE0\xF2\xFC "
            "\xE2 c++."
        );

        Wrapper replace(
            &doc,
            &TextProcessor::replace_words,
            {
                {"old_word", "\xEF\xF0\xE8\xE2\xE5\xF2"},
                {"new_word", "\xE7\xE4\xF0\xE0\xE2\xF1\xF2\xE2\xF3\xE9"}
            }
        );

        std::cout << replace.execute() << "\n\n";
    }

    std::cout << "2. " << "\xCF\xEE\xE8\xF1\xEA \xF1\xEB\xEE\xE2\xEE\xF1\xEE\xF7\xE5\xF2\xE0\xED\xE8\xFF:" << '\n';

    {
        TextProcessor doc(base_text);

        Wrapper find(
            &doc,
            &TextProcessor::find_phrase,
            { {"phrase", "\xEF\xF0\xEE\xE2\xE5\xF0\xEA\xE0 \xEF\xF0\xEE\xE3\xF0\xE0\xEC\xEC\xFB"} }
        );

        std::cout << find.execute() << "\n\n";
    }

    std::cout << "3. " << "\xC7\xE0\xE3\xEB\xE0\xE2\xED\xFB\xE5 \xE1\xF3\xEA\xE2\xFB \xF1\xEB\xEE\xE2:" << '\n';

    {
        TextProcessor doc(base_text);
        Wrapper words(&doc, &TextProcessor::capitalize_words, {});
        std::cout << words.execute() << "\n\n";
    }

    std::cout << "4. " << "\xC7\xE0\xE3\xEB\xE0\xE2\xED\xFB\xE5 \xE1\xF3\xEA\xE2\xFB \xEF\xF0\xE5\xE4\xEB\xEE\xE6\xE5\xED\xE8\xE9:" << '\n';

    {
        TextProcessor doc(base_text);
        Wrapper sentences(&doc, &TextProcessor::capitalize_sentences, {});
        std::cout << sentences.execute() << "\n\n";
    }

    std::cout << "5. " << "\xCD\xF3\xEC\xE5\xF0\xE0\xF6\xE8\xFF \xEF\xF0\xE5\xE4\xEB\xEE\xE6\xE5\xED\xE8\xE9:" << '\n';

    {
        TextProcessor doc(base_text);
        Wrapper numbering(&doc, &TextProcessor::number_sentences, {});
        std::cout << numbering.execute() << '\n';
    }

    return 0;
}



