#include <iostream>
#include <sstream>
#include <fstream>

class Parser
{
public:
   std::string instructions; // change to string strem?
   std::string filename;     // to pass to CodeWriter
   std::istringstream stream;
   std::string current_command;
   std::string arithmetic[9] = {"add", "sub", "neg", "eq", "gt", "lt", "and", "or", "not"};

   Parser()
   { // opens the file and parses the data in a string
      std::fstream filehandle;
      do
      { // opens the file

         std::cout << "\n Enter file name to translate (without extension), or enter exit \n ";
         std::getline(std::cin, filename);
         if (filename == "exit")
            exit(0);

         filehandle.open(filename + ".vm", std::ifstream::in | std::ifstream::binary);
      } while (!(filehandle.is_open()));

      filehandle.seekg(0, std::ios::end); // grabs file size
      instructions.resize(filehandle.tellg());
      filehandle.seekg(std::ios::beg);

      filehandle.read(&instructions[0], instructions.size()); // copy all data to instructions
      filehandle.close();

      std::cout << "File Read Successfully. \n"
                << "********************** ";
      cleaner();
   };

   void cleaner()
   {                 // removes whitespace and comments from the file
      char *dp, *sp; // destination and string pointer
      int i = 0;
      dp = sp = &instructions[0];

      do
      { // clear consecutive empty space and comments //find a way to deal with carriage FUCKING RETURN

         while (*dp == '\r' || *dp == '\f' || *dp == '\v' || *dp == '\t')
            dp++; // skip through whitespace characters and carriage return
         while (*dp == ' ' && *(dp + 1) == ' ')
            dp++; // clear consecutive whitespace
         while (*dp == '/' && *(dp + 1) == '/')
         {
            while (*dp != '\n')
               dp++;
         } // skip till next line if comments(//) <<<<only till comment end as we have inline comments too
         i++;

      } while (*sp++ = *dp++); // puts the character pointed by(*dp) in the location pointed by string pointer(*sp), thus rewriting string.
                               //  Increments both to next char, until dp returns NULL.
      instructions.resize(i);

      i = 0;
      dp = sp = &instructions[0];

      do
      { // clear empty lines
         if (*(&instructions[0]) == '\r' && *(&instructions[1]) == '\n')
         {
            dp++;
            dp++;
         } // deal with this
         if (*(&instructions[0]) == '\n')
         {
            dp++;
         } // deal with this
         while (*dp == '\n' && *(dp - 1) == '\n')
            dp++; // clear consecutive newlines
         i++;
      } while (*sp++ = *dp++);

      instructions.resize(i - 1); // deal with /n again
      std::cout << instructions;
      stream.str(instructions);
      std::cout << "\nInstructions cleaned \n"
                << "********************** \n";

      // the next function
   }

   bool hasMoreCommands()
   { // returns 1 if more commands exist, else 0; put commands in CurrentCommand
      if (std::getline(stream, current_command))
         return 1;
      else
         return 0;
   }

   const std::string commandType()
   {
      std::string command = current_command.substr(0, current_command.find(' ')); // grab first word
      if (command == "push")
         return "C_PUSH";
      if (command == "pop")
         return "C_POP";
      if (command == "label")
         return "C_LABEL";
      if (command == "goto")
         return "C_GOTO";
      if (command == "if-goto")
         return "C_IF";
      if (command == "function")
         return "C_FUNCTION";
      if (command == "call")
         return "C_CALL";
      if (command == "return")
         return "C_RETURN";
      for (int i = 0; i <= sizeof(arithmetic) / sizeof(std::string); i++)
      {
         if (command == arithmetic[i])
            return "C_ARITHMETIC";
      }
      return "NULL"; // else
   }
   std::string arg1(const std::string type)
   {
      if (type == "C_ARITHMETIC")
         return current_command.substr(0, current_command.find(' '));
      if (type == "C_LABEL" || type == "C_GOTO" || type == "C_IF")
         return current_command.substr((current_command.find(' ') + 1));
      if (type == "C_POP" || type == "C_PUSH" || type == "C_FUNCTION" || type == "C_CALL")
      {
         std::string temp = current_command.substr((current_command.find(' ') + 1));
         return temp.substr(0, (temp.find(' ')));
      }
      else
         return "";
   }
   int arg2(const std::string type)
   {
      if (type == "C_POP" || type == "C_PUSH" || type == "C_FUNCTION" || type == "C_CALL") // remove this
      {
         std::string temp = current_command.substr((current_command.find(' ') + 1));
         return std::stoi(temp.substr((temp.find(' '))));
      }
      else
         return NULL;
   }
};

class CodeWriter
{
public:
   CodeWriter(std::string filename)
   {
      std::cout << filename << std::endl;
   }
};

int main()
{
   Parser input; // opens file and cleans
   CodeWriter output(input.filename);

   while (input.hasMoreCommands())
   {
   }
}