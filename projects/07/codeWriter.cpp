#include <iostream>
#include <fstream>
#include "VMTranslator.h"

write::codeWriter::codeWriter(std::string file)
{
    filename = file; // for global use
    log.open(filename + "_log_file.txt", std::ostream::out | std::ostream::binary | std::ofstream::trunc);
    o_file_handle.open(filename + ".asm", std::ofstream::out | std::ostream::binary | std::ofstream::trunc);
}

void write::codeWriter::writeArithmetic(std::string command)
{
}

void write::codeWriter::writePushPop(std::string type, std::string segment, int index)
{
    if (type == "C_PUSH")
    {
        std::string command;
        std::string push_template = std::string("@SP\n") + // pushes value to stack and increments stack
                                    "A=M\n" +
                                    "M=D\n" +
                                    "@SP\n" +
                                    "M=M+1\n";
        if (segment == "constant")
        {
            command = '@' + std::to_string(index) + '\n' +
                      "D=A\n" +
                      push_template;
        }

        else if (segment == "local" || segment == "argument" || segment == "this" || segment == "that")
        {
            std::string seg;
            (segment == "local") ? seg = "LCL" : (segment == "argument") ? seg = "ARG"
                                             : (segment == "this")       ? seg = "THIS"
                                             : (segment == "that")       ? seg = "THAT"
                                                                         : seg = "";

            command = "@" + seg + '\n' +
                      "A=A+" + std::to_string(index) + '\n' +
                      "D=M\n" +
                      push_template;
        }

        else if (segment == "static")
        {
            command = '@' + filename + '.' + std::to_string(index) + '\n' +
                      "D=M\n" +
                      push_template;
        }

        else if (segment == "temp")
        {
            if (index > 7)
            {
                std::cout << "Static index " << index << "is out of scope";
                close(1);
            }
            command = '@' + std::to_string(5 + index) + '\n' +
                      "A=M\n" +
                      "D=M\n" +
                      push_template;
        }

        else if (segment == "pointer")
        {
            if (!(index == 0 || index == 1))
            {
                std::cout << "Only Pointer 0/1 is allowed";
                close(1);
            }
            if (index == 0)
            {
                command = std::string("@THIS\n") +
                          "D=M\n" +
                          push_template;
            }
            else if (index == 1)
            {
                command = std::string("@THAT\n") +
                          "D=M\n" +
                          push_template;
            }
        }

        // write to file
        o_file_handle << command;
        log << "\n//PUSH " + segment + ' ' + std::to_string(index) + '\n' + command;
    }

    else // if C_POP
    {
        std::string command;
        std::string pop_template = std::string("@SP\n") + // pushes value to stack and increments stack
                                   "AM=M-1\n" +
                                   "D=M\n";

        // empty stack pop; use variable?
        if (segment == "local" || segment == "argument" || segment == "this" || segment == "that")
        {
            std::string seg;
            (segment == "local") ? seg = "LCL" : (segment == "argument") ? seg = "ARG"
                                             : (segment == "this")       ? seg = "THIS"
                                             : (segment == "that")       ? seg = "THAT"
                                                                         : seg = "";

            command = pop_template +
                      "@" + seg + '\n' +
                      "A=A+" + std::to_string(index) + '\n' +
                      "M=D\n";
        }

        else if (segment == "static")
        {
            command = pop_template +
                      '@' + filename + '.' + std::to_string(index) + '\n' +
                      "M=D\n";
        }

        else if (segment == "temp")
        {
            if (index > 7)
            {
                std::cout << "Static index " << index << "is out of scope";
                close(1);
            }
            command = pop_template +
                      '@' + std::to_string(5 + index) + '\n' +
                      "A=M\n" +
                      "M=D\n";
        }

        else if (segment == "pointer")
        {
            if (!(index == 0 || index == 1))
            {
                std::cout << "Only Pointer 0/1 is allowed";
                close(1);
            }
            if (index == 0)
            {
                command = pop_template +
                          std::string("@THIS\n") +
                          "M=D\n";
            }
            else if (index == 1)
            {
                command = pop_template +
                          std::string("@THAT\n") +
                          "M=D\n";
            }
        }

        // write to file
        o_file_handle << command;
        log << "\n//POP " + segment + ' ' + std::to_string(index) + '\n' + command;
    }
}

int write::codeWriter::close(bool flag) // close the files
{
    if (flag)
    {
        std::cout << "\nError found, terminating translation.\nCheck command after last translated from log file\n";
        o_file_handle.close();
        log.close();
        exit(1);
    }
    else
    {
        std::string end_loop;
        end_loop = "(END)\n@END\n0;JMP";

        o_file_handle << end_loop;
        log << "\n//END LOOP\n" + end_loop;

        o_file_handle.close();
        log.close();
        std::cout << "File and log_file Assembled with same name successfully \n"
                  << "********************** \n";
        return 0;
    }
}