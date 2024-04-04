#include <iostream>
#include <sstream>
#include <fstream>
#include "VMTranslator.h"
#include <filesystem>

parse::Parser::Parser()
{ // intializes file / dir for injection
    std::cout << "\n Enter file name with extension, or directory name to begin translation \n Enter exit to terminate \n ";
    std::getline(std::cin, filename);
    if (filename == "exit")
        exit(0);

    if (!std::filesystem::exists(filename))
    {
        std::cout << filename << " doesn't exist as a file or directory, exiting\n";
        exit(1);
    }

    // if file
    if (filename.find(".vm") != std::string::npos)
    {
        path << filename;
        filename.resize(filename.find('.')); // grab filename
    }

    // if dir
    else
    {
        std::string var;
        for (auto const &dir : std::filesystem::directory_iterator(filename))
        {
            stream << dir.path() << std::endl;
            stream.seekg(1, stream.beg);
            std::getline(stream, var, '"'); // deals with quotations around dir fetched by path()
            if (var.find(".vm") != std::string::npos)
            {
                path << var << std::endl;
                files++; // maintains counter of .vm files
            }
            stream.str("");
        }
    }
    std::cout << path.str() << std::endl; // injested dir for reference
};

const std::string parse::Parser::fileHandler()
{ // opens a file(s) and dumps the data in a string
    std::string var;
    std::getline(path, var);
    std::fstream filehandle;

    filehandle.open(var, std::ifstream::in | std::ifstream::binary);

    filehandle.seekg(0, std::ios::end);      // grabs file size
    instructions.resize(filehandle.tellg()); // resize the string
    filehandle.seekg(std::ios::beg);         // reset file pointer

    filehandle.read(&instructions[0], instructions.size()); // copy all data to instructions
    filehandle.close();
    parse::Parser::cleaner();

    if (files != 0) // if directory
        var = var.substr(var.find('\\') + 2);
    var.resize(var.find('.'));
    std::cout << var << " read and processed";

    return var; // return current filename
}

void parse::Parser::cleaner() // removes whitespace and comments from the file
{
    char *dp, *sp; // destination and string pointer
    int i = 0;
    dp = sp = &instructions[0];

    do
    { // clear consecutive empty space and comments //find a way to deal with carriage FUCKING RETURN

        while (*dp == '\r' || *dp == '\f' || *dp == '\v' || *dp == '\t')
            dp++; // skip through whitespace characters and carriage return
        while (*dp == ' ' && *(dp + 1) == ' ')
            dp++;                              // clear consecutive whitespace
        while (*dp == '/' && *(dp + 1) == '/') // skip comments
        {
            while (*dp != '\n')
                dp++;
        } // skip till next line if comments(//) <<<<only till comment end as we have inline comments too
        i++;

    } while (*sp++ = *dp++); // puts the character pointed by(*dp) in the location pointed by string pointer(*sp), thus rewriting string.
                             //  Increments both to next char, until dp returns NULL.
    instructions.resize(i);

    i = 0;
    dp = sp = &instructions[0];

    do
    { // clear empty lines
        if (*(&instructions[0]) == '\r' && *(&instructions[1]) == '\n')
        {
            dp++;
            dp++;
        } // deal with this
        if (*(&instructions[0]) == '\n')
        {
            dp++;
        }
        while (*dp == '\n' && *(dp - 1) == '\n')
        {
            dp++; // clear consecutive newlines
        }
        i++;
    } while (*sp++ = *dp++);

    instructions.resize(i - 1); // deal with /n again //needs empty line in vm file for some reason
    // std::cout << instructions << std::endl;
    stream.str(instructions);
    stream.clear(); // clears flags to avoid clash with getline()
}

bool parse::Parser::hasMoreCommands()
{ // returns 1 if more commands exist, else 0; put commands in CurrentCommand
    if (std::getline(stream, current_command))
    {
        return 1;
    }
    else
        return 0;
}

const std::string parse::Parser::commandType()
{
    std::string command = current_command.substr(0, current_command.find(' ')); // grab first word
    if (command == "push")
        return "C_PUSH";
    if (command == "pop")
        return "C_POP";
    if (command == "label")
        return "C_LABEL";
    if (command == "goto")
        return "C_GOTO";
    if (command == "if-goto")
        return "C_IF";
    if (command == "function")
        return "C_FUNCTION";
    if (command == "call")
        return "C_CALL";
    if (command == "return")
        return "C_RETURN";
    for (int i = 0; i <= sizeof(arithmetic) / sizeof(std::string); i++)
    {
        if (command == arithmetic[i])
            return "C_ARITHMETIC";
    }
    return "NULL"; // else
}
std::string parse::Parser::arg1(const std::string type) // returns first argument
{
    if (type == "C_ARITHMETIC")
        return current_command.substr(0, current_command.find(' '));
    if (type == "C_LABEL" || type == "C_GOTO" || type == "C_IF")
        return current_command.substr((current_command.find(' ') + 1));
    if (type == "C_POP" || type == "C_PUSH" || type == "C_FUNCTION" || type == "C_CALL")
    {
        std::string temp = current_command.substr((current_command.find(' ') + 1));
        return temp.substr(0, (temp.find(' ')));
    }
    else
        return "";
}
int parse::Parser::arg2(const std::string type) // returns second argument
{
    std::string temp = current_command.substr((current_command.find(' ') + 1));
    return std::stoi(temp.substr((temp.find(' '))));
}