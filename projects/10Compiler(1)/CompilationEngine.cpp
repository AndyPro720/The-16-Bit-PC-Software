#include "JackAnalyzer.h"
#include <fstream>

analyzer::CompilationEngine::CompilationEngine(std::stringstream &path)
{

    analyzer::JackTokenizer token(path);
    std::string filename = token.filename_g;

    filename.resize(filename.find('.'));
    filehandle.open(filename + "_c" + ".xml", std::ofstream::out | std::ofstream::trunc);

    filehandle << "<token>\n";
    while (token.hasMoreTokens())
        token.hasMoreTokens();

    std::cout << "here";
}

void analyzer::CompilationEngine::CompileClass()
{
}

void analyzer::CompilationEngine::Close(bool flag)
{

    filehandle << "</token>";
    filehandle.close();
}