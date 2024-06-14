#include "JackAnalyzer.h"
#include <fstream>
#include <sstream>

analyzer::JackTokenizer::JackTokenizer(std::stringstream &path)
{ // opens a file(s) and dumps the data in a string
    std::string fileName;
    std::getline(path, fileName);
    std::fstream filehandle;

    filehandle.open(fileName, std::ifstream::in | std::ifstream::binary);

    filehandle.seekg(0, std::ios::end);      // grabs file size
    instructions.resize(filehandle.tellg()); // resize the string
    filehandle.seekg(std::ios::beg);         // reset file pointer

    filehandle.read(&instructions[0], instructions.size()); // copy all data to instructions
    filehandle.close();

    fileName = fileName.substr(fileName.find('\\') + 2);
    fileName.resize(fileName.find('.'));
    std::cout << fileName << " read and processed" << std::endl;
}
