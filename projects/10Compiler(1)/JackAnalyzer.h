#ifndef JACK_ANAYLYZER
#define JACK_ANAYLYZER

#if __cplusplus < 201703L
#error "C++17 or greater required for compilation\n"
#endif

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

namespace analyzer
{
    class JackAnalyzer
    {
    public:
        JackAnalyzer();
        std::stringstream path;
        int fileCount = 0;

        const std::string fileHandler();
    };

    class JackTokenizer
    {
    public:
        JackTokenizer(std::stringstream &path);
        std::string filename_g;
        char current_token[100]{};
        char *fp, *sp, *wp;
        std::string instructions, type;

        bool hasMoreTokens();
        const std::string tokenType();
    };

    class CompilationEngine
    {
    private:
        analyzer::JackTokenizer token;

    public:
        CompilationEngine(std::stringstream &path);
        std::fstream filehandle;

        void CompileClass();
        void Close(bool flag);
    };
}

#endif