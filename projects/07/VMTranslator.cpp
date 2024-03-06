#include <iostream>
#include "VMTranslator.h"

int main()
{
   parse::Parser input; // opens file and cleans it

   write::codeWriter output(input.filename);

   while (input.hasMoreCommands())
   {
      std::cout << input.current_command << std::endl;
   }
}