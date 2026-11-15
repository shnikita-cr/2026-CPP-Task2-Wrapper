#pragma once

#include "Wrapper.h"
#include <string>
#include <unordered_map>
#include <memory>
#include <iostream>


class Engine {
std::unordered_map<std::string, std::unique_ptr<BaseWrapper>> Wrappers;

public:
    void register_command(std::unique_ptr<BaseWrapper> wrap, const std::string& commandName);

    std::any execute(const std::string& commandName, const ArgDict& arguments);
};