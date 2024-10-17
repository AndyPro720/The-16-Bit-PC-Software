#include "JackAnalyzer.h"
#include <fstream>

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
    }
    indent(-1);
    writeData("close", "", "class");
}

void analyzer::CompilationEngine::Close(bool flag)
{

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
