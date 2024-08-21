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
    // input.str(instructions);
    filehandle.close();

    fileName = fileName.substr(fileName.find('\\') + 2);
    fileName.resize(fileName.find('.'));
    std::cout << fileName << " read and processed" << std::endl;
    sp = fp = &instructions[0];
}

bool analyzer::JackTokenizer::hasMoreTokens()
{
    wp = &current_token[0];
    do // break for each token
    {
        // std::cout << "ran" << std::endl;

        if (*fp == '/' && *(fp + 1) == '/')
        {
            while (*fp != '\n')
                fp++;
            sp = fp;
            continue;
        }

        else if (*fp == '/' && *(fp + 1) == '*' && *(fp + 2) == '*')
        {
            fp = fp + 4;
            while (*fp != '/' && *(fp - 1) != '*')
                fp++;
            fp++; // skip newline
            sp = fp;
            continue;
        }

        else if (*fp == '\"') // string constant
        {
            fp++;
            while (*(fp) != '\"')
                *wp++ = *fp++;
            break;
        }

        if (*fp == '\n' || *fp == '\r')
        {
            *fp++;
            continue;
        }

        if (*fp == ' ')
        {
            fp++;
            continue;
        }
        fp++; // if none

    } while (*fp);

    std::cout << ":" << current_token << ":" << std::endl;

    return 0;
}
