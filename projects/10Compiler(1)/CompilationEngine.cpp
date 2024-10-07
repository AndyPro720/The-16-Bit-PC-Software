#include "JackAnalyzer.h"
#include <fstream>

analyzer::CompilationEngine::CompilationEngine(std::string filename)
{

    filename.resize(filename.find('.'));
    filehandle.open(filename + "_c" + ".xml", std::ofstream::out | std::ofstream::trunc);

    filehandle << "<token>\n";
}

void analyzer::CompilationEngine::Close(bool flag)
{

    filehandle << "</token>";
    filehandle.close();
}