#ifndef VMT_H
#define VMT_H

#include <string>
#include <fstream>
#include <sstream>

namespace parse
{
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

namespace write
{
    class codeWriter
    {
    public:
        std::fstream o_file_handle;
        std::ofstream log;
        std::string filename;
        int count = 0; // for labels

        codeWriter(std::string file);
        void writePushPop(std::string type, std::string segment, int index);
        void writeArithmetic(std::string type);
        int close(bool flag);
    };

}

#endif