#include<iostream>
#include<sstream>
#include<fstream>

class Parser {
   public:
   std::string instructions; //change to string strem?
   std::istringstream stream;
   std::string current_command = "hello";

   Parser() { //opens the file and parses the data in a string
      std::string filename;
      std::fstream filehandle;
      do { //opens the file

         std::cout<<"\n Enter file name to translate (without extension), or enter exit \n ";
         std::getline(std::cin, filename);
         if(filename == "exit") exit(0);

         filehandle.open(filename + ".vm", std::ifstream::in | std::ifstream::binary);
      }while(!(filehandle.is_open()));
      

      filehandle.seekg(0, std::ios::end); //grabs file size
      instructions.resize(filehandle.tellg());
      filehandle.seekg(std::ios::beg);

      filehandle.read(&instructions[0], instructions.size()); //copy all data to instructions
      filehandle.close();

      std::cout << "File Read Successfully. \n" << "********************** ";
      cleaner();

   };


   void cleaner() { //removes whitespace and comments from the file
      char *dp, *sp;      //destination and string pointer
      int i = 0;
      dp = sp = &instructions[0];

    do {        //clear consecutive empty space and comments //find a way to deal with carriage FUCKING RETURN

        while(*dp == '\r' || *dp == '\f' || *dp == '\v'|| *dp == '\t')  dp++;   //skip through whitespace characters and carriage return
        while(*dp == ' ' && *(dp+1) == ' ') dp++;    //clear consecutive whitespace                                                                      
        while(*dp == '/' && *(dp+1) == '/') { while(*dp != '\n') dp++; dp++;}      //skip till next line if comments(//)
        i++;

    }while(*sp++ = *dp++);      //puts the character pointed by(*dp) in the location pointed by string pointer(*sp), thus rewriting string.
                                // Increments both to next char, until dp returns NULL.
   instructions.resize(i);

   i = 0;
   dp = sp = &instructions[0];

   do {        //clear empty lines 
      if(*(&instructions[0]) == '\r' && *(&instructions[1]) == '\n') {dp++; dp++;}  //deal with this
      while(*dp == '\n' && *(dp-1) == '\n') dp++;    //clear consecutive newlines                                                                      
      i++;
   }while(*sp++ = *dp++);

   instructions.resize(i-2); //deal with /r/n again
   std::cout<<instructions;
   stream.str(instructions);
   std::cout<<"\nInstructions cleaned \n"<< "********************** \n";
   
  //the next function 

   }

bool hasMoreCommands() { //returns 1 if more commands exist, else 0; put commands in CurrentCommand
    if(std::getline(stream, current_command)) return 1; 
    else return 0;

}

const std::string commandType() {
    std::string arithmetic [9] = {"add", "sub", "neg", "eq", "gt", "lt", "and", "or", "not"};
    std::string command = current_command.substr(0, current_command.find(' '));
    std::cout<<'\n'<<command;
    if(command == "push") return "C_PUSH";
    //else return "POP";
   
}
   

};

int main() {
   Parser intial;
   while(intial.hasMoreCommands()) {
     if(intial.commandType() == "C_PUSH") std::cout<<"eagle has landed"; 
     //else std::cout<<"bleh";
   }
   
}