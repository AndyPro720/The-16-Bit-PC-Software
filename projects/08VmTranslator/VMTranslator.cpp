#include <iostream>
#include "VMTranslator.h"

int main()
{
   parse::Parser input; // opens file and cleans it

   write::codeWriter output(input.filename); // creates output assembly and logfile

   while (input.hasMoreCommands())
   {
      std::string type = input.commandType();

      if (type == "C_ARITHMETIC")
      {
         output.writeArithmetic(input.arg1(type));
      }
      else if (type == "C_PUSH" || type == "C_POP")
      {
         output.writePushPop(type, input.arg1(type), input.arg2(type));
      }

      else if (type == "NULL")
      {
         std::cout << input.current_command << " is an invalid command, exiting!" << std::endl;
         output.close(1);
      }
   }
   return output.close(0);
}