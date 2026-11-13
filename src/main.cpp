#include "engine.h"
#include "text_document.h"
#include "wrapper.h"

#include <cstdio>
#include <exception>
#include <iostream>
#include <string>
#include <string_view>
#include <utility>

namespace
{

class OrderSummary
{
  public:
    explicit OrderSummary(std::string heading) : heading_(std::move(heading))
    {
    }

    [[nodiscard]] std::string describe(int quantity, const std::string& item, bool gift) const
    {
        return heading_ + ": " + std::to_string(quantity) + " " + item +
               (gift ? " (gift)" : " (regular)");
    }

  private:
    std::string heading_;
};

void print_result(std::string_view label, const std::string& result)
{
    std::cout << label << ":\n" << result << "\n\n";
}

void demonstrate_architecture()
{
    TextDocument document(u8"cat sleeps. dog runs! ёж идёт? cat watches.");
    OrderSummary order("Order");

    Wrapper replace(&document, &TextDocument::replace_words, {{"from", "cat"}, {"to", "fox"}});
    Wrapper find(&document, &TextDocument::find_phrase, {{"phrase", "fox sleeps"}});
    Wrapper capitalize_words(&document, &TextDocument::capitalize_words, {});
    Wrapper capitalize_sentences(&document, &TextDocument::capitalize_sentences, {});
    Wrapper number_sentences(&document, &TextDocument::number_sentences, {});
    Wrapper order_wrapper(&order, &OrderSummary::describe,
                          {{"quantity", 3}, {"item", "books"}, {"gift", true}});

    Engine engine;
    engine.register_command(&replace, "replace");
    engine.register_command(&find, "find");
    engine.register_command(&capitalize_words, "capitalize_words");
    engine.register_command(&capitalize_sentences, "capitalize_sentences");
    engine.register_command(&number_sentences, "number_sentences");
    engine.register_command(&order_wrapper, "order");

    print_result("Initial document", document.text());
    print_result("replace (defaults)", engine.execute("replace"));
    print_result("replace (override from=dog)", engine.execute("replace", {{"from", "dog"}}));
    print_result("find (defaults)", engine.execute("find"));
    print_result("find (override phrase=missing phrase)",
                 engine.execute("find", {{"phrase", "missing phrase"}}));
    print_result("capitalize_sentences", engine.execute("capitalize_sentences"));
    print_result("capitalize_words", engine.execute("capitalize_words"));
    print_result("number_sentences", engine.execute("number_sentences"));
    print_result("order (defaults)", engine.execute("order"));
    print_result("order (override quantity=4, gift=false)",
                 engine.execute("order", {{"quantity", 4}, {"gift", false}}));
}

} // namespace

int main()
{
    try
    {
        demonstrate_architecture();
        return 0;
    }
    catch (const std::exception& error)
    {
        std::fprintf(stderr, "Error: %s\n", error.what());
    }
    catch (...)
    {
        std::fputs("Error: unknown exception\n", stderr);
    }

    return 1;
}
