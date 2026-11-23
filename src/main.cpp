#include <iostream>
#include <string>
#include "wrapper.h"
#include "engine.h"
#include "strchanger.h"

int main(){
    std::string TestStr("lorem ipsum dolor sit amet, consectetur adipiscing elit. sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.");

    StringChanger StrCh(TestStr);

    Wrapper wrapper_Find(&StrCh, &StringChanger::Find, { {"arg1",  std::string("it is")}});
    Wrapper wrapper_Replace(&StrCh, &StringChanger::Replace, { {"arg1",  std::string("MainStr")}, {"arg2",  std::string("NewStr")} });
    Wrapper wrapper_CapWords(&StrCh, &StringChanger::CapitalizeWords, {  });
    Wrapper wrapper_CapSens(&StrCh, &StringChanger::CapitalizeSentences, {  });
    Wrapper wrapper_NumSens(&StrCh, &StringChanger::NumberSentences, {  });

    Engine engine;
    engine.register_command(&wrapper_Find, "find");
    engine.register_command(&wrapper_Replace, "replace");
    engine.register_command(&wrapper_CapWords, "capwords");
    engine.register_command(&wrapper_CapSens, "capsens");
    engine.register_command(&wrapper_NumSens, "numsens");

    std::cout << engine.execute("find", {  {"arg1", std::string("ut labore et")}}) << "\n\n";
    std::cout << engine.execute("find", {  {"arg1", std::string("ut labore ets")}}) << "\n\n";
    std::cout << engine.execute("replace", {  {"arg1", std::string("mollit")}, {"arg2", std::string("ne mollit")} }) << "\n\n";
    std::cout << engine.execute("capwords") << "\n\n";
    std::cout << engine.execute("capsens") << "\n\n";
    std::cout << engine.execute("numsens") << "\n\n";
    return 0; 
}