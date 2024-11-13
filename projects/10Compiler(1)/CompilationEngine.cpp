#include "JackAnalyzer.h"
#include <fstream>

// change token.currenttoken comparisons to string(token...)
// continute working on statements
namespace
{
    std::fstream *handle; // using this as aren't multi-threading(instances)
    int indentLevel = 0;
    void writeData(const std::string &type, std::string data, std::string angleString);
    void indent(int indent);

} // namespace

analyzer::CompilationEngine::CompilationEngine(std::stringstream &path) : token(path)
{
    std::string filename = token.filename_g;

    filename.resize(filename.find('.'));
    filehandle.open(filename + "_c" + ".xml", std::ofstream::out | std::ofstream::trunc);
    handle = &filehandle;

    CompileClass();
}

void analyzer::CompilationEngine::CompileClass()
{
    if (token.hasMoreTokens() && std::string(token.current_token) == "class")
    {

        writeData("angled", " ", token.current_token);
        indent(1);
        writeData("angled-data", token.current_token, token.tokenType());
        token.hasMoreTokens(); //  class name
        writeData("angled-data", token.current_token, token.tokenType());
        token.hasMoreTokens(); //  {
        writeData("angled-data", token.current_token, token.tokenType());
    }
    else
        Close(0); // throw error

    while (token.hasMoreTokens() && std::string(token.current_token) != "}")
    {
        if (std::string(token.current_token) == "static" || std::string(token.current_token) == "field")
            CompileClassVarDec();
        else
            CompileSubroutineDec();
    }

    indent(-1);
    writeData("close", "", "class");
}

void analyzer::CompilationEngine::CompileClassVarDec()
{

    writeData("angled", "", "classVarDec");
    indent(1);
    writeData("angled-data", token.current_token, token.tokenType()); // static || field
    token.hasMoreTokens();
    writeData("angled-data", token.current_token, token.tokenType()); // type
    token.hasMoreTokens();
    writeData("angled-data", token.current_token, token.tokenType()); // var name

    while (token.hasMoreTokens() && std::string(token.current_token) == ",")
    {
        writeData("angled-data", token.current_token, token.tokenType()); // ,
        token.hasMoreTokens();
        writeData("angled-data", token.current_token, token.tokenType()); //  var name
    }

    writeData("angled-data", token.current_token, token.tokenType()); // ;
    indent(-1);
    writeData("close", "", "classVarDec");
}

void analyzer::CompilationEngine::CompileSubroutineDec()
{
    writeData("angled", "", "subroutineDec");
    indent(1);
    writeData("angled-data", token.current_token, token.tokenType()); // constructor || function || method
    token.hasMoreTokens();
    writeData("angled-data", token.current_token, token.tokenType()); // void || type
    token.hasMoreTokens();
    writeData("angled-data", token.current_token, token.tokenType()); // name
    token.hasMoreTokens();
    writeData("angled-data", token.current_token, token.tokenType()); // (

    CompileParameterList();
    if (token.current_token != ")")
        token.hasMoreTokens();
    writeData("angled-data", token.current_token, token.tokenType()); // )

    token.hasMoreTokens();
    CompileSubroutineBody();

    indent(-1);
    writeData("close", "", "subroutineDec");
}

void analyzer::CompilationEngine::CompileParameterList()
{

    writeData("angled", "", "ParameterList");
    indent(1);
    while (token.tokenType() == "keyword")
    {
        writeData("angled-data", token.current_token, token.tokenType()); // type
        token.hasMoreTokens();
        writeData("angled-data", token.current_token, token.tokenType()); // name
        token.hasMoreTokens();
        if (std::string(token.current_token) == ")")
            break;
        writeData("angled-data", token.current_token, token.tokenType()); // ,
        token.hasMoreTokens();
    }
    indent(-1);
    writeData("close", "", "ParameterList");
}

void analyzer::CompilationEngine::CompileSubroutineBody()
{
    writeData("angled", "", "SubroutineBody");
    indent(1);
    writeData("angled-data", token.current_token, token.tokenType()); // {
    token.hasMoreTokens();
    while (std::string(token.current_token) == "var")
    {
        CompileVarDec();
        token.hasMoreTokens();
    }

    CompileStatements();
    writeData("angled-data", token.current_token, token.tokenType()); // }
    indent(-1);
    writeData("close", "", "SubroutineBody");
}

void analyzer::CompilationEngine::CompileVarDec()
{

    writeData("angled", "", "VarDec");
    indent(1);
    writeData("angled-data", token.current_token, token.tokenType()); // var
    token.hasMoreTokens();
    writeData("angled-data", token.current_token, token.tokenType()); // type
    token.hasMoreTokens();
    writeData("angled-data", token.current_token, token.tokenType()); // varName
    token.hasMoreTokens();
    while (token.current_token == ",")
    {
        writeData("angled-data", token.current_token, token.tokenType()); // varName
        token.hasMoreTokens();
    }
    writeData("angled-data", token.current_token, token.tokenType()); // ;
    indent(-1);
    writeData("close", "", "VarDec");
}

void analyzer::CompilationEngine::CompileStatements()
{
    writeData("angled", "", "statements");
    indent(1);

    indent(-1);
    writeData("close", "", "statements");
}

void analyzer::CompilationEngine::Close(bool flag)
{

    if (flag == 0)
        std::cout << "Error with Syntax, aborting" << std::endl;

    filehandle.close();
}

namespace
{
    void writeData(const std::string &type, std::string data, std::string angleString)
    {
        indent(0);
        if (type == "angled")
            *handle << '<' + angleString + ">\n";

        else if (type == "angled-data")
        {
            *handle << '<' + angleString + ">";
            *handle << ' ' + data + ' ';
            *handle << '<' + angleString + ">\n";
        }
        else if (type == "close")
            *handle << "</" + angleString + ">\n";

        else
            *handle << data + '\n';
    }

    void indent(int indent) // 1 to increment, -1 to decrement, 0 to write
    {
        if (indent == 1)
            indentLevel++;
        else if (indent == -1)
            indentLevel--;
        else
        {

            for (int i = indentLevel; i > 0; i--)
            {
                *handle << '\t';
            }
        }
    }
}
