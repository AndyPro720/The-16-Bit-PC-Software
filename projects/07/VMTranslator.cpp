#include <iostream>
#include <sstream>
#include <fstream>
#include "VMTranslator.h"

#if 0
class CodeWriter
{
public:
   CodeWriter(std::string filename)
   {

      std::fstream o_file_handle;
      std::ofstream log(filename + "_log_file.txt", std::ostream::out | std::ostream::binary | std::ofstream::trunc);
      o_file_handle.open(filename + ".asm", std::ofstream::out | std::ostream::binary | std::ofstream::trunc);
   }

   void wrtieArithmetic(std::string commad)
   {
   }
};
#endif

int main()
{
   parse::Parser input; // opens file and cleans

   // CodeWriter output(input.filename);

   while (input.hasMoreCommands())
   {
      std::cout << input.current_command << std::endl;
   }
}