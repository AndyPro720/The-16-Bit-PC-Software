/* JackAnalyzer.exe takes input a Jack file / directory and provides parsed/analyzed output in XML files
 * this header file provides a good overview of the structure of the project

Intended to be run for Jack the object oriented language, created in tandem with  Hack, the custom 16-Bit-PC
*/

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
        char *fp, *bp, *wp;
        std::string instructions, type;

        bool hasMoreTokens(int flag = 0);
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
        void CompileClassVarDec();
        void CompileSubroutineDec();
        void CompileParameterList();
        void CompileSubroutineBody();
        void CompileVarDec();
        void CompileStatements();
        void CompileLet();
        void CompileIf();
        void CompileWhile();
        void CompileDo();
        void CompileReturn();
        void CompileExpression();
        void CompileTerm();
        void CompileExpressionList();
        void Close(bool flag);
    };
}

#endif