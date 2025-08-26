/* Jack Tokenizer extracts tokens from supplied Jack files, along with few other nifity functions
 * Run via JackCompiler.exe

Intended to be run for Jack the object oriented language, created in tandem with  Hack, the custom 16-Bit-PC
*/

#include "JackCompiler.h"
#include <fstream>
#include <sstream>
#include <string>

namespace // private functions declaration
{
    bool isSymbol(char c);
    bool isKeyword(const std::string keyword);
    bool isIdentifier(char id[]);
}

analyzer::JackTokenizer::JackTokenizer(const std::string inputFile)
{ // opens a file(s) and dumps the data in a string
    std::fstream filehandle;

    filehandle.open(inputFile, std::ifstream::in | std::ifstream::binary);

    filehandle.seekg(0, std::ios::end);      // grabs file size
    instructions.resize(filehandle.tellg()); // resize the string
    filehandle.seekg(std::ios::beg);         // reset file pointer

    filehandle.read(&instructions[0], instructions.size()); // copy all data to instructions
    filehandle.close();

    std::cout << "File read successfully: " << inputFile << std::endl;
    // print name for reference
    std::string name = inputFile;
    name = name.substr(name.find('\\') + 2);
    name.resize(name.find('.'));
    std::cout << name << " read and processed" << std::endl;
    fp = &instructions[0];
}

bool analyzer::JackTokenizer::hasMoreTokens(int flag)
{
    std::fill(current_token, current_token + 100, 0); // clear string
    wp = &current_token[0];

    if (flag == 1) // store state
        bp = fp;

    else if (flag == 2) // backtrack to last stored state
    {
        fp = bp;
        hasMoreTokens(); //  update current_token
        return true;
    }

    while (*fp) // break for each token
    {

        if (*fp == '/' && *(fp + 1) == '/') // ignore line comments
        {
            while (*fp != '\n')
                fp++;
            continue;
        }

        else if (*fp == '/' && *(fp + 1) == '*' && *(fp + 2) == '*') // ignore multi-line commends
        {
            fp = fp + 4;
            while (!(*fp == '/' && *(fp - 1) == '*'))
                fp++;
            fp++; // skip newline
            continue;
        }

        else if (*fp == '\"') // string constant
        {
            fp++;
            while (*(fp) != '\"')
                *wp++ = *fp++;
            fp++;
            type = "stringConstant";
            break;
        }

        if (*fp == '\n' || *fp == '\r') // ignore line breaks
        {
            fp++; // remove asterik!?
            continue;
        }

        if (*fp == ' ' || *fp == '\r' || *fp == '\f' || *fp == '\v' || *fp == '\t') // ignore whitespace
        {
            fp++;
            continue;
        }

        else if (isSymbol(*fp)) // if symbol
        {

            *wp++ = *fp++; // grab a symbol

            type = "symbol";
            break;
        }

        else if (isdigit(*fp)) // if integerConstant
        {
            do
            {
                *wp++ = *fp++;
            } while (isdigit(*fp));
            type = "integerConstant";
            break;
        }

        do // grab a token
        {
            *wp++ = *fp++;
        } while (!(*fp == ' ' || isSymbol(*fp)));

        if (isKeyword(current_token))
        {
            type = "keyword";
            break;
        }
        if (isIdentifier(current_token))
        {
            type = "identifier";
            break;
        }
    }
    if ((*fp) == '\0') // if EOF
        return false;

#if 0
    std::cout << ":" << current_token << ":";
    std::cout << tokenType() << std::endl;
#endif

    return true;
}

const std::string analyzer::JackTokenizer::tokenType()
{
    return type;
}

namespace
{

    bool isSymbol(char c)
    {
        if (c == '{' || c == '}' || c == '(' || c == ')' || c == '[' || c == '.' || c == ',' || c == ';' || c == '+' || c == '-' || c == '*' || c == '/' || c == '&' || c == '|' || c == '<' || c == '>' || c == '=' || c == '~' || c == ']')
            return true;
        else
            return false;
    }

    bool isKeyword(const std::string keyword)
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