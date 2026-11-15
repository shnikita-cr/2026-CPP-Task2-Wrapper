#ifndef WRAPPER_TEXT_SENTENCES_H
#define WRAPPER_TEXT_SENTENCES_H

#include <cstddef>
#include <string>
#include <utility>
#include <vector>

namespace text
{

using SentenceRange  = std::pair<std::size_t, std::size_t>;
using SentenceRanges = std::vector<SentenceRange>;

[[nodiscard]] SentenceRanges sentence_ranges(const std::u32string& value);

} // namespace text

#endif // WRAPPER_TEXT_SENTENCES_H
