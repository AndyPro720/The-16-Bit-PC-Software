#include <iostream>
#include "VMTranslator.h"

int main()
{
   parse::Parser input; // opens file and cleans it

   write::codeWriter output(input.filename); // creates output assembly and logfile

   while (input.hasMoreCommands())
   {
      std::string type = input.commandType();

      std::cout << input.current_command << type << std::endl;
      if (type == "C_ARITHMETIC")
      {
         output.writeArithmetic(input.current_command);
      }
      else if (type == "C_PUSH" || type == "C_POP")
      {
         output.writePushPop(input.current_command);
      }

      else if (type == "NULL")
      {
         std::cout << input.current_command << " is an invalid command, exiting!" << std::endl;
         std::cout << "the line is  " << __LINE__ << std::endl;
         exit(1);
      }
   }
   return output.close();
}