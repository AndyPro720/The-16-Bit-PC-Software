#include <iostream>
#include "VMTranslator.h"

int main()
{
   parse::Parser input;                      // intializes file/dir
   write::codeWriter output(input.filename); // creates output assembly and logfile

   do
   {
      std::string current_file = input.fileHandler(); // go to next file
      while (input.hasMoreCommands())
      {
         std::string type = input.commandType();

         if (type == "C_ARITHMETIC")
         {
            output.writeArithmetic(input.arg1(type));
         }
         else if (type == "C_PUSH" || type == "C_POP")
         {
            output.writePushPop(type, input.arg1(type), input.arg2(type), current_file);
         }

         else if (type == "NULL")
         {
            std::cout << input.current_command << " is an invalid command, exiting!" << std::endl;
            output.close(1);
         }
      }
      std::cout << "\nTranslated " << current_file << " successfully"
                << "\n********************** " << std::endl;
   } while (--input.files > 0);
   return output.close(0);
}
