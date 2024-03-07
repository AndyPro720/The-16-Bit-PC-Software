#include <iostream>
#include <fstream>
#include "VMTranslator.h"

write::codeWriter::codeWriter(std::string filename)
{
    log.open(filename + "_log_file.txt", std::ostream::out | std::ostream::binary | std::ofstream::trunc);
    o_file_handle.open(filename + ".asm", std::ofstream::out | std::ostream::binary | std::ofstream::trunc);
}

void write::codeWriter::writeArithmetic(std::string command)
{
    //
}

void write::codeWriter::writePushPop(std::string type, std::string segment, int index)
{
    if (type == "C_PUSH")
    {
        std::string command;
        if (segment == "constant")
        {
            command = '@' + std::to_string(index) + '\n' +
                      "D=A\n" +
                      "@SP\n" +
                      "A=M\n" +
                      "M=D\n" +
                      "@SP\n" +
                      "M=M+1\n";
        }

        else if (segment == "local" || segment == "argument" || segment == "this" || segment == "that")
        {

            std::string seg;
            (segment == "local") ? seg = "LCL" : (segment == "argument") ? seg = "ARG"
                                             : (segment == "this ")      ? seg = "THIS"
                                             : (segment == "that")       ? seg = "THAT"
                                                                         : seg = "";

            command = "@" + seg + '\n' +
                      "A=A+" + std::to_string(index) + '\n' +
                      "D=M\n" +
                      "@SP\n" +
                      "A=M\n" +
                      "M=D\n" +
                      "@SP\n" +
                      "M=M+1\n";
        }

        // write to file
        o_file_handle << command;
        log << "\n//PUSH " + segment + ' ' + std::to_string(index) + '\n' + command;
    }
    else
    {
        // POP
    }
}

int write::codeWriter::close() // close the files
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