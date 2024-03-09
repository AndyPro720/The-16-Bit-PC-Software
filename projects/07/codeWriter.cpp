#include <iostream>
#include <fstream>
#include "VMTranslator.h"

write::codeWriter::codeWriter(std::string file)
{
    filename = file; // for global use
    log.open(filename + "_log_file.txt", std::ostream::out | std::ostream::binary | std::ofstream::trunc);
    o_file_handle.open(filename + ".asm", std::ofstream::out | std::ostream::binary | std::ofstream::trunc);
}

void write::codeWriter::writeArithmetic(std::string type)
{
    std::string command;
    std::string arith_template = std::string("@SP\n") +
                                 "AM=M-1\n" +
                                 "D=M\n" +
                                 "A=A-1\n";
    if (type == "add" || type == "sub" || type == "and" || type == "or")
    {
        std::string symbol;
        (type == "add") ? symbol = "+" : (type == "sub") ? symbol = "-"
                                     : (type == "and")   ? symbol = "&"
                                     : (type == "or")    ? symbol = "|"
                                                         : symbol = "";

        command = arith_template + "M=M" + symbol + "D\n";
    }

    else if (type == "neg" || type == "not")
    {
        std::string symbol;
        (type == "neg") ? symbol = "-" : (type == "not") ? symbol = "!"
                                                         : symbol = "";
        command = std::string("@SP\n") +
                  "A=M-1\n" +
                  "M=" + symbol + "M\n";
    }

    else if (type == "eq" || type == "gt" || type == "lt") // return -1 if true and 0 if false
    {                                                      // create one single (true  after jmp?)
        std::string symbol;
        (type == "eq") ? symbol = ";JEQ\n" : (type == "gt") ? symbol = ";JGT\n"
                                         : (type == "lt")   ? symbol = ";JLT\n"
                                                            : symbol = "";

        command = arith_template +
                  "D=M-D\n" + // store diff
                  "@TRUE." + std::to_string(count) + '\n' +
                  "D" + symbol +

                  "@SP\n" // if false
                  "A=M-1\n" +
                  "M=0\n" +
                  "@NEXT." + std::to_string(count) + '\n' +
                  "0;JMP\n" +

                  "(TRUE." + std::to_string(count) + ")\n" + // if true
                  "@SP\n"
                  "A=M-1\n" +
                  "M=-1\n" +
                  "(NEXT." + std::to_string(count) + ")\n";
        count++;
    }

    o_file_handle << command;
    log << "\n// " + type + '\n' + command;
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

            command = "@" + std::to_string(index) + '\n' +
                      "D=A\n" +
                      "@" + seg + '\n' +
                      "A=M+D\n" +
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
                std::cout << "Temp index " << index << "is out of scope";
                close(1);
            }
            command = '@' + std::to_string(5 + index) + '\n' +
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
        std::string pop_template = std::string("@SP\n") + // pops value from stack and decrement sp
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

            if (index == 0) // fetch value from stack and push to segment
            {
                command = pop_template +
                          "@" + seg + '\n' +
                          "A=M\n" +
                          "M=D\n";
            }
            else // fetch value from stack and push to segment offset
            {
                command = "@" + std::to_string(index) + '\n' +
                          "D=A\n" +
                          "@" + seg + '\n' +
                          "D=D+M\n" +
                          "@R13\n" +
                          "M=D\n" +      // store segment pointer in R13
                          pop_template + // pop value from stack
                          "@R13\n"
                          "A=M\n"
                          "M=D\n"; // store value in segment pointer
            }
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
                std::cout << "Temp index " << index << "is out of scope";
                close(1);
            }
            command = pop_template +
                      '@' + std::to_string(5 + index) + '\n' +
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