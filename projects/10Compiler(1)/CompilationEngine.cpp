#include "JackAnalyzer.h"
#include <fstream>

// inhertiance not vs share object as private member

analyzer::CompilationEngine::CompilationEngine(std::stringstream &path) : token(path)
{
    std::string filename = token.filename_g;

    filename.resize(filename.find('.'));
    filehandle.open(filename + "_c" + ".xml", std::ofstream::out | std::ofstream::trunc);

    filehandle << "<token>\n";
    CompileClass();

    std::cout << "here";
}

void analyzer::CompilationEngine::CompileClass()
{
    while (token.hasMoreTokens())
        std::cout << token.current_token << std::endl;
}

void analyzer::CompilationEngine::Close(bool flag)
{

    filehandle << "</token>";
    filehandle.close();
}