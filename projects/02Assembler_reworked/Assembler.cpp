#include<iostream> 
#include<fstream> 

class assemble {
 
   std::string file_name;
   std::string raw_instruction; 
   
   public:

   std::string file_mgmt() {     //method for I/O file management

   std::fstream i_file_handle;
   std::fstream o_file_handle;
   while (true)   //open input file
   {
    std::cout << "Input the file name (without extension)    -----       ctrl+c to exit\n";
    std::getline(std::cin, file_name);
    i_file_handle.open(file_name+".asm", std::ifstream::in); 

   if (i_file_handle.is_open()) {
    std::cout << "File Opened sucessfully \n";
    
    o_file_handle.open(file_name+".hack", std::ofstream::out | std::ofstream::trunc);
    std::cout << "Created the output file, processing assembly";

    return file_name;
   }
   else {
    std::cout << "Couldn't find the named file.\n";
   }
   }
}

}; 




int main()
    {
       assemble code;
       
       code.file_mgmt();
       return 0;
    }