#include <iostream>
#include <fstream>
#include "VMTranslator.h"

write::codeWriter::codeWriter(std::string filename)
{

    std::fstream o_file_handle;
    std::ofstream log(filename + "_log_file.txt", std::ostream::out | std::ostream::binary | std::ofstream::trunc);
    o_file_handle.open(filename + ".asm", std::ofstream::out | std::ostream::binary | std::ofstream::trunc);
}