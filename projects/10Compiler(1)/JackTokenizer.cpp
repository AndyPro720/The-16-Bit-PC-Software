#include "JackAnalyzer.h"
#include <fstream>
#include <sstream>

namespace // private functions declaration
{
    bool isSymbol(char c);
    bool isKeyword(char keyword[]);
    bool isIdentifier(char id[]);
}

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

        if (*fp == '/' && *(fp + 1) == '/') // ignore line comments
        {
            while (*fp != '\n')
                fp++;
            sp = fp;
            continue;
        }

        else if (*fp == '/' && *(fp + 1) == '*' && *(fp + 2) == '*') // ignore multi-line commends
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

        if (*fp == '\n' || *fp == '\r') // ignore line breaks
        {
            *fp++;
            continue;
        }

        if (*fp == ' ') // ignore whitespace
        {
            fp++;
            continue;
        }

        else if (isSymbol(*fp)) // if symbol
        {
            *wp++ = *fp++;
            break;
        }

        else if (isdigit(*fp)) // if integerConstant
        {
            do
            {
                *wp++ = *fp++;
            } while (isdigit(*fp));
            break;
        }

        do // grab a string
        {
            *wp++ = *fp++;
        } while (*fp != ' ');

        if (isKeyword(current_token))
            break;
        if (isIdentifier(current_token))
            break;

        else
            std::cout << "Error processing token " << current_token << "or" << *fp << std::endl;
        return 1;

        fp++; // if none

    } while (*fp);

    std::cout << ":" << current_token << ":" << std::endl;

    return 0;
}

namespace
{

    bool isSymbol(char c)
    {
        if (c == '{' || c == '}' || c == '(' || c == ')' || c == '[' || c == '.' || c == ',' || c == ';' || c == '+' || c == '-' || c == '*' || c == '/' || c == '&' || c == '|' || c == '|' || c == '<' || c == '>' || c == '=' || c == '~')
            return true;
        else
            return false;
    }

    bool isKeyword(char keyword[])
    {
        if (keyword == "class" || keyword == "constructor" || keyword == "function" || keyword == "method" || keyword == "field" || keyword == "static" || keyword == "var" || keyword == "int" || keyword == "char" || keyword == "boolean" || keyword == "void" || keyword == "true" || keyword == "false" || keyword == "null" || keyword == "this" || keyword == "let" || keyword == "do" || keyword == "if" || keyword == "else" || keyword == "while" || keyword == "return")
            return true;
        else
            return false;
    }

    bool isIdentifier(char id[])
    {
        if (isdigit(id[0]))
            return false;
        else
            return true;
    }
}