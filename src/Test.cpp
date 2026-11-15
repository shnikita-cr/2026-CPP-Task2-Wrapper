#include "Test.h"

int Test::execute(const std::string& sss, int iii, bool bbb) { // NOLINT(readability-convert-member-functions-to-static)
    std::cout << sss << " " << iii << " " << bbb << "\n";
    return iii;
}