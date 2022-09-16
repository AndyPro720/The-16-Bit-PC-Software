#include<iostream>
#include<fstream>

int main() {

 std::fstream i_file_handle;
 std::string file_name;
 std::string instructions;

                  std::cout << "Input the file name (without extension)    <----->       ctrl+c to exit\n";
                  std::getline(std::cin, file_name);
                  i_file_handle.open(file_name+".asm", std::ifstream::in | std::ios::binary); 

                     i_file_handle.seekg(0, std::ios::end);    //get data in instructions and close file. 
                        std::cout << i_file_handle.tellg() << std::endl;
                        instructions.resize(i_file_handle.tellg(), ' ');
                        i_file_handle.seekg(std::ios::beg);
                        std::cout << i_file_handle.tellg();
                        i_file_handle.read(&instructions.at(0), instructions.size()); //read the file and store it

                        if (i_file_handle.fail())                      // check for EOF
    std::cout << "[fail reached]\n";
  else
    std::cout << "[ reading]\n";
                        std::cout << "File Read Successfully.. \n" << "********************** \n" ; 
                        //std::cout << instructions;
                        if(i_file_handle) std::cout << "here";
                        else std::cout << "not here";
                        i_file_handle.close(); 

         std::fstream o_file_handle;
         o_file_handle.open(file_name + ".hack", std::ofstream::out | std::ofstream::binary | std::ofstream::trunc);
         
         o_file_handle << instructions; 
         o_file_handle.close();
         std::cout << "File Assembled with same name successfully \n" << "********************** \n" ;

         return 0;
   } 