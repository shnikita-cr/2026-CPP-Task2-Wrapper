#include "TextProcessor.h"

// NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
std::string TextProcessor::replace_words(const std::string& from, const std::string& p_to) const
{
    if (from.empty())
    {
        return text_;
    }

    std::string result;
    result.reserve(text_.size());
    std::size_t pos = 0;
    while (pos < text_.size())
    {
        auto found = text_.find(from, pos);
        if (found == std::string::npos)
        {
            result.append(text_, pos, std::string::npos);
            break;
        }
        result.append(text_, pos, found - pos);
        result += p_to;
        pos = found + from.size();
    }
    return result;
}

std::string TextProcessor::find_phrase(const std::string& phrase) const
{
    return text_.find(phrase) != std::string::npos ? std::string("Found") : std::string("Not found");
}

std::string TextProcessor::capitalize_words() const
{
    std::string result = text_;
    bool word_start = true;
    for (char& cha : result)
    {
        const auto ucha = static_cast<unsigned char>(cha);
        if ((bool)std::isalpha(ucha))
        {
            if (word_start)
            {
                cha = static_cast<char>(std::toupper(ucha));
            }
            word_start = false;
        }
        else
        {
            word_start = true;
        }
    }
    return result;
}

std::string TextProcessor::capitalize_sentences() const
{
    std::string result = text_;
    bool sentence_start = true;
    for (char& cha : result)
    {
        const auto ucha = static_cast<unsigned char>(cha);
        if (cha == '.' || cha == '!' || cha == '?')
        {
            sentence_start = true;
        }
        else if (sentence_start)
        {
            if ((bool)std::isalpha(ucha))
            {
                cha = static_cast<char>(std::toupper(ucha));
                sentence_start = false;
            }
            else if (!(bool)std::isspace(ucha))
            {
                sentence_start = false;
            }
        }
    }
    return result;
}

std::string TextProcessor::number_sentences() const
{
    std::ostringstream out;
    std::string current;
    std::size_t num = 1;
    for (char cha : text_)
    {
        current += cha;
        if (cha == '.' || cha == '!' || cha == '?')
        {
            out << num++ << ". " << trim(current) << '\n';
            current.clear();
        }
    }
    const std::string tail = trim(current);
    if (!tail.empty())
    {
        out << num << ". " << tail << '\n';
    }
    return out.str();
}