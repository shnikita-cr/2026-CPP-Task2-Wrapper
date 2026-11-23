#pragma once
#include <string>

class StringChanger {
private:
	std::string Str;
public:
	StringChanger(std::string &str) : Str(str) {}
	std::string Replace(const std::string& From, const std::string& Tor);
	bool Find(const std::string& What);
	std::string CapitalizeWords();
	std::string CapitalizeSentences();
	std::string NumberSentences();
};