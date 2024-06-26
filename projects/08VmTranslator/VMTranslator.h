#ifndef VMT_H
#define VMT_H

#if __cplusplus < 201703L
#error "C++17 or greater required for compilation\n"
#endif

#include <string>
#include <fstream>
#include <sstream>

namespace parse
{
    class Parser
    {
    public:
        Parser();
        int files = 0; // counter for vm files
        std::string instructions;
        std::string filename;
        std::stringstream stream;
        std::stringstream path;
        std::string current_command;
        std::string arithmetic[9] = {"add", "sub", "neg", "eq", "gt", "lt", "and", "or", "not"};

        void cleaner();
        const std::string fileHandler();
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
        int count = 0;       // for labels in writePushPop()
        int returnCount = 0; // counter for function return address labels

        codeWriter(std::string file);
        void writeArithmetic(std::string type);
        void writePushPop(std::string type, std::string segment, int index, std::string current_file);
        void writeLabel(std::string label);
        void writeGoto(std::string label);
        void writeIf(std::string label);
        void writeFunction(std::string functionName, int numVars);
        void writeCall(std::string functionName, int numArgs);
        void Return();
        int close(bool flag);
    };

}

#endif