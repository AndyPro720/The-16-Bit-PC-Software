/* VMTranslator takes input a Virtual Machine Language(.vm) file / directory of files and provides
translated assembly output

Intended to be run on Hack, the custom 16-Bit-PC
*/

#include <iostream>
#include "VMTranslator.h"

int main()
{
   parse::Parser input;                      // intializes file/dir
   write::codeWriter output(input.filename); // creates output assembly and log_file

   do // translates instructions across files and writes to output file
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
         else if (type == "C_LABEL")
         {
            output.writeLabel(input.arg1(type));
         }
         else if (type == "C_GOTO")
         {
            output.writeGoto(input.arg1(type));
         }
         else if (type == "C_IF")
         {
            output.writeIf(input.arg1(type));
         }
         else if (type == "C_FUNCTION")
         {
            output.writeFunction(input.arg1(type), input.arg2(type));
         }
         else if (type == "C_CALL")
         {
            output.writeCall(input.arg1(type), input.arg2(type));
         }
         else if (type == "C_RETURN")
         {
            output.Return();
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
