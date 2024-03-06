#include <iostream>
#include <fstream>
#include "VMTranslator.h"

write::codeWriter::codeWriter(std::string filename)
{
    // rework
    std::fstream o_file_handle;
    std::ofstream log(filename + "_log_file.txt", std::ostream::out | std::ostream::binary | std::ofstream::trunc);
    o_file_handle.open(filename + ".asm", std::ofstream::out | std::ostream::binary | std::ofstream::trunc);
}

void write::codeWriter::writeArithmetic(std::string command)
{
}

void write::codeWriter::writePushPop(std::string command)
{
}

int write::codeWriter::close()
{
    return 0;
    // close the file
}