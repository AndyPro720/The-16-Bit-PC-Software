#ifndef VMT_H
#define VMT_H

namespace parse
{
#include <string>
#include <sstream>
    class Parser
    {
    public:
        Parser();
        std::string instructions; // change to string strem?
        std::string filename;     // to pass to CodeWriter
        std::istringstream stream;
        std::string current_command;
        std::string arithmetic[9] = {"add", "sub", "neg", "eq", "gt", "lt", "and", "or", "not"};

        void cleaner();
        bool hasMoreCommands();
        const std::string commandType();
        std::string arg1(const std::string type);
        int arg2(const std::string type);
    };
}

#endif