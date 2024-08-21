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
        std::stringstream input;
        char current_token[50]{};
        char *fp, *sp, *wp;
        std::string instructions;

        bool hasMoreTokens();
    };
}

#endif