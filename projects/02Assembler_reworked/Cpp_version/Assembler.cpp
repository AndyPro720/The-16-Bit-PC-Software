#include<iostream> 
#include<fstream> 
#include<sstream>
#include<algorithm>
#include<unordered_map>

class assemble {
   
   std::string instructions; 
   std::string log_file;
   std::unordered_map <std::string, std::string> symbol;
   std::unordered_map <std::string, std::string> dest;
   std::unordered_map <std::string, std::string> comp;
   std::unordered_map <std::string, std::string> jmp;

   public:
   assemble();
   bool io_state = false;

   int file_handler() {     //method for file I/O management

   std::string file_name;

      if (io_state == false) {  //Input file handler
            std::fstream i_file_handle;
             while (true)   //open file    
               {
                  std::cout << "Input the file name (without extension)    <----->       ctrl+c to exit\n";
                  std::getline(std::cin, file_name);
                  i_file_handle.open(file_name+".asm", std::ifstream::in | std::ifstream::binary); 

                     if (i_file_handle.seekg(0, std::ios::end)) {   //get data in instructions and close file. 
                        
                        instructions.resize(i_file_handle.tellg());
                        i_file_handle.seekg(0, std::ios::beg);
                        i_file_handle.read(&instructions[0], instructions.size()); //read the file and store it

                        std::cout << "File Read Successfully. \n" << "********************** \n" ; 
                        i_file_handle.close(); 
                        return 0;
                        }

                     else {
                         std::cout << "Couldn't find the named file.\n" << "********************** \n" ;
                        }
                }
         }
      else {  //Output file handler
         std::fstream o_file_handle;
         std::ofstream log(file_name + "_log_file.txt", std::ostream::out | std::ostream::binary | std::ofstream::trunc);
         o_file_handle.open(file_name + ".hack", std::ofstream::out | std::ostream::binary | std::ofstream::trunc);
         
         o_file_handle << instructions; 
         log << log_file;

         o_file_handle.close();
         log.close();
         std::cout << "File and log_file Assembled with same name successfully \n" << "********************** \n" ;
      }
         return 0;
   }   

   void cleaner() {      //method to clear whitespace and comments from code

      instructions.erase(std::remove_if(instructions.begin(), instructions.end(), ::isblank), instructions.end());   //clears all whitespace
      instructions.erase(std::remove_if(instructions.begin(), instructions.end(),  [](unsigned char x){ if(x == '\r') return 1; else return 0; }), instructions.end());   //removes \r line endings(windows systems)

      std::istringstream stream(instructions);
      std::string line;
      instructions.clear();

      while(std::getline(stream, line)) {   //cleans comments

        
         if (!line.size()) continue;

         else if (line.find("//") != std::string::npos) {
            if(line.find("//") == 0) continue;
            instructions += line.substr(0, line.find("//")) + '\n'; 
         }
         else {
             instructions += line + '\n';
         }
      }

      instructions.erase(instructions.end()-1);  //trims the last newline

      std::cout << "Instructions Cleaned \n" << "********************** \n";
   }
   
   void first_pass_labels() {     //parses through instructions and stores labels and their values
 
      std::istringstream stream(instructions);
      std::string line;
      int instr_no = 0;  
      instructions.clear();
      
      while(std::getline(stream, line)) {        

         if(line.find('(') != std::string::npos) {   //stores labels
            symbol[line.substr(1, line.find(')')-1)] = std::to_string(instr_no);
         }

         else {    //regular instructions
            instructions += line + '\n';
            instr_no++;
         }
      }
      instructions.erase(instructions.end()-1);  //trims the last newline

      std::cout << "First Pass Completed \n" << "********************** \n";
   }
   
   void second_pass_var() {   //stores and replaces variables. Replaces label mentions
      
      std::istringstream stream(instructions);
      std::string line;
      std::string key;
      int reg = 15;  //variables memory starts from 15+
      instructions.clear();
      
      while(std::getline(stream, line)) {
         if(line[0] == '@' && !std::isdigit(line[1])) {    //if A instruction and non register declaration
            key = line.substr(1);

            //inserts variable and value, ignores duplicates
            if(symbol.find(key) == symbol.end()) {
                symbol[key] = std::to_string(++reg); 
            }

            line.replace(1, line.size(), symbol.at(key));   //replaces label/variables with numerical value
            instructions += line + '\n';
         }   

         else instructions += line + '\n';
      }


      instructions.erase(instructions.end()-1);  //trims the last newline
      std::cout << "Second Pass Completed, variables stored \n" << "********************** \n";
   }

   void translator() {     //translates Assembly instructions to 16 bit binary
      std::istringstream stream(instructions);
      std::string line;
      instructions.clear();
      std::string binary; 
      
      while(getline(stream, line)) {

         if(line[0] == '@') {    //if A instruction
            int n = std::stoi(line.substr(1, line.size()));
            binary.clear();

            while(n!=0) {     //convert n to binary
               binary = (n%2==0 ?"0":"1") + binary; 
               n/=2; 
            }

            std::string padding(16-binary.size(), '0');   //adds padding to binary
            binary = padding + binary;
            instructions += binary + '\n';
            log_file += line + " : " + binary + '\n';
         } 

         else {   //dest=comp+jmp  if C instruction
            std::string d = "";
            std::string c = "";
            std::string j = "";
            int a = 0;
            int b = line.size();

            if(line.find('=') != std::string::npos)  { d = line.substr(0, line.find('=')); a = line.find('=')+1; }  //if dest exists
            if(line.find(';') != std::string::npos)  { j = line.substr(line.find(';')+1, line.size()); b = line.find(';'); } //if jmp exists 
            c = line.substr(a, b);
            
            instructions += "111" + comp[c] + dest[d] + jmp[j] + '\n'; //translation
            log_file += line + " : " + dest[d] + " dest " + comp[c] + " comp " + jmp[j] + " jmp " + '\n';
         }

      }

      instructions.erase(instructions.end()-1);  //trims the last newline
      log_file.erase(log_file.end()-1);  //trims the last newline

      std::cout << "Translation compeleted \n" << "********************** \n";
      io_state = true; //for file_handler()

   }

}; 


int main()
    {
       assemble code;
       
       code.file_handler();
       code.cleaner();
       code.first_pass_labels();
       code.second_pass_var();
       code.translator();
       code.file_handler();

       return 0;
    }


assemble::assemble() {  //constructor for storing maps
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
   
  dest = {
      {"", "000"},
      {"M", "001"},
      {"D", "010"},
      {"MD", "011"},
      {"A", "100"},
      {"AM", "101"},
      {"AD", "110"},
      {"AMD", "111"},
  }; 

   comp = {
      {"0", "0101010"},
      {"1", "0111111"},
      {"-1", "0111010"},
      {"D", "0001100"},
      {"A", "0110000"},
      {"M", "1110000"},
      {"!D", "0001101"},
      {"!A", "0110001"},
      {"!M", "1110001"},
      {"-D", "0001111"},
      {"-A", "0110011"},
      {"-M", "1110011"},
      {"D+1", "0011111"},
      {"A+1", "0110111"},
      {"M+1", "1110111"},
      {"D-1", "0001110"},
      {"A-1", "0110010"},
      {"M-1", "1110010"},
      {"D+A", "0000010"},
      {"D+M", "1000010"},
      {"D-A", "0010011"},
      {"D-M", "1010011"},
      {"A-D", "0000111"},
      {"M-D", "1000111"},
      {"D&A", "0000000"},
      {"D&M", "1000000"},
      {"D|A", "0010101"},
      {"D|M", "1010101"}, 
   };

   jmp = {
      {"", "000"},
      {"JGT", "001"},
      {"JEQ", "010"},
      {"JGE", "011"},
      {"JLT", "100"},
      {"JNE", "101"},
      {"JLE", "110"},
      {"JMP", "111"},
   };

}