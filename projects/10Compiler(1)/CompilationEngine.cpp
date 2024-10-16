#include "JackAnalyzer.h"
#include <fstream>

namespace
{
    int indentLevel = 0;
    void writeData(const std::string &type, std::string data, std::string angleString, std::fstream &handle);
    void indent(std::fstream &handle, bool indent);

} // namespace

analyzer::CompilationEngine::CompilationEngine(std::stringstream &path) : token(path)
{
    std::string filename = token.filename_g;

    filename.resize(filename.find('.'));
    filehandle.open(filename + "_c" + ".xml", std::ofstream::out | std::ofstream::trunc);

    CompileClass();
}

void analyzer::CompilationEngine::CompileClass()
{
#if 0
    while (token.hasMoreTokens())
        std::cout << token.current_token << '#' << std::endl;
    std::cout << token.current_token;
#endif
    if (token.hasMoreTokens())
    {
        writeData("angled", " ", token.current_token, filehandle);
        indent(filehandle, 1);
        writeData("angled-data", " name of class ", token.current_token, filehandle);
        indent(filehandle, 0);
        writeData("close", " name of class ", token.current_token, filehandle);
    }
}

void analyzer::CompilationEngine::Close(bool flag)
{

    filehandle.close();
}

namespace
{
    void writeData(const std::string &type, std::string data, std::string angleString, std::fstream &handle)
    {
        if (type == "angled")
            handle << '<' + angleString + ">\n";

        else if (type == "angled-data")
        {
            handle << '<' + angleString + ">";
            handle << ' ' + data + ' ';
            handle << '<' + angleString + ">\n";
        }
        else if (type == "close")
            handle << "</" + angleString + ">\n";

        else
            handle << data + '\n';
    }

    void indent(std::fstream &handle, bool indent)
    {
        if (indent)
            indentLevel++;
        else
            indentLevel--;

        for (int i = indentLevel; i > 0; i--)
        {
            handle << '\t';
        }
    }
}
