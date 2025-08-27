/* JackCompiler.exe takes input a Jack file / directory and provides parsed/analyzed output in XML files
 * this header file provides a good overview of the structure of the project
Intended to be run for Jack the object oriented language, created in tandem with  Hack, the custom 16-Bit-PC
*/

#ifndef JACK_COMPILER
#define JACK_COMPILER

#if __cplusplus < 201703L
#error "C++17 or greater required for compilation\n"
#endif

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <unordered_map>

namespace analyzer // redundant from jackanalyzer.cpp
{
    enum class symbolKind;
    enum class segment;
    enum class arithmetic;

    class JackCompiler
    {
    public:
        JackCompiler();
        std::stringstream path;
        int fileCount = 0;

        const std::string fileHandler();
    };

    class JackTokenizer
    {
    public:
        JackTokenizer(const std::string inputFile);
        char current_token[100]{};
        char *fp, *bp, *wp;
        std::string instructions, type;

        bool hasMoreTokens(int flag = 0);
        const std::string tokenType();
    };

    class SymbolTable
    {
    private:
        struct symbolInfo
        {
            std::string type;
            symbolKind kind;
            int index;
        };
        std::unordered_map<std::string, symbolInfo> classSymbols;
        std::unordered_map<std::string, symbolInfo> subroutineSymbols;
        int staticCount;
        int fieldCount;
        int argCount;
        int varCount;

    public:
        SymbolTable();
        void StartSubroutine();
        void Define(const std::string &name, const std::string &type, symbolKind kind);
        int VarCount(symbolKind kind);
        symbolKind KindOf(const std::string &name);
        std::string TypeOf(const std::string &name);
        int IndexOf(const std::string &name);
    };

    class VMWriter
    {
    public:
        std::fstream fhandle;
        VMWriter(const std::string &inputFile);

        void WritePush(segment seg, int index);
        void WritePop(segment seg, int index);
        void WriteArithmetic(arithmetic command);
        void WriteLabel(const std::string &label);
        void WriteGoto(const std::string &label);
        void WriteIf(const std::string &label);
        void WriteCall(const std::string &name, int nArgs);
        void WriteFunction(const std::string &name, int nLocals);
        void WriteReturn();
        void Close();
    };

    class CompilationEngine
    {
    private:
        analyzer::JackTokenizer &token;
        analyzer::SymbolTable symbolTable;
        analyzer::VMWriter &vmWriter;
        std::string className;
        int labelCount;

    public:
        CompilationEngine(JackTokenizer &token, VMWriter &vmWriter);
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
        int CompileExpressionList();
    };

    enum class symbolKind
    {
        STATIC,
        FIELD,
        ARG,
        VAR,
        NONE
    };

    enum class segment
    {
        CONST,
        ARG,
        LOCAL,
        STATIC,
        THIS,
        THAT,
        POINTER,
        TEMP
    };

    enum class arithmetic
    {
        ADD,
        SUB,
        NEG,
        EQ,
        GT,
        LT,
        AND,
        OR,
        NOT
    };

}
#endif