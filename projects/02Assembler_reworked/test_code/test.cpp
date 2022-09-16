#include<iostream>
#include<fstream>
#include<sstream>
#include<algorithm>
#include<unordered_map>

int main() {

  std::unordered_map <std::string, std::string> symbol;
  symbol  = {
      {"R0", "0"},
      {"R1", "1"},
      {"R2", "2"},
      {"R3", "3"},
      {"R4", "4"},
      {"R5", "5"},
      {"R6", "6"},
      {"R7", "7"},
      {"R8", "8"},
      {"R9", "9"},
      {"R10", "10"},
      {"R11", "11"},
      {"R12", "12"},
      {"R13", "13"},
      {"R14", "14"},
      {"R15", "15"},
      {"SCREEN", "16384"},
      {"KBD", "24576"},
      {"SP", "0"},
      {"LCL", "1"},
      {"ARG", "2"},
      {"THIS", "3"},
      {"THAT", "4"}, 
   };

   std::fstream i_file_handle;
   std::string instructions;
   int reg = 15;
         
                i_file_handle.open("test.txt", std::ifstream::in); 

                        i_file_handle.seekg(0, std::ios::end); 
                        instructions.resize(i_file_handle.tellg());
                        i_file_handle.seekg(std::ios::beg);
                        i_file_handle.read(&instructions[0], instructions.size()); //read the file and store it
                     
                     std::istringstream stream(instructions);
                     std::string line;
                     instructions.clear();
                     while(std::getline(stream, line)) {
                       if(line[0] == '@' && !std::isdigit(line[1])) {    //if A instruction and non register declaration
            std::string key = line.substr(1);
            //inserts variable and value, ignores duplicates
            if(symbol.find(key) == symbol.end()) {
                symbol[line.substr(1, line.size())] = std::to_string(++reg); 
                //std::cout << key;

            }
            line.replace(1, line.size(), symbol.at(line.substr(1, line.size())));   //replaces label/variables with numerical value
            instructions += line + '\n';
         }   

         else instructions += line + '\n';
      }

            for(auto it = symbol.cbegin(); it != symbol.cend(); ++it)
      {
        //std::cout << it->first << " :"<< it->second << " " << std::endl; 
      }
      instructions.erase(instructions.end()-1);  //trims the last newline
      std::cout << " \nSecond Pass Completed, variables stored \n" << "********************** \n";

}