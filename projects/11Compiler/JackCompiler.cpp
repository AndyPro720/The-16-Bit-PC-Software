/* JackCompiler takes input a Jack file / directory and provides parsed/analyzed output in XML files
 * Run via JackCompiler.exe

Intended to be run for Jack the object oriented language, created in tandem with  Hack, the custom 16-Bit-PC
*/

#include <filesystem>
#include "JackCompiler.h"

analyzer::JackCompiler::JackCompiler()
{ // intializes file / dir for injection
    std::string filename;
    std::cout << "\n Enter file name with extension, or directory name to begin translation \n Enter exit to terminate \n ";
    std::getline(std::cin, filename);
    if (filename == "exit")
        exit(0);

    if (!std::filesystem::exists(filename))
    {
        std::cout << filename << " doesn't exist as a file or directory, exiting\n";
        exit(1);
    }
    // if file
    if (filename.find(".jack") != std::string::npos)
    {
        path << filename;
        filename.resize(filename.find('.')); // grab filename
    }

    // if dir
    else
    {
        std::string var;
        std::stringstream stream;
        for (auto const &dir : std::filesystem::directory_iterator(filename))
        {
            stream << dir.path() << std::endl;
            stream.seekg(1, stream.beg);
            std::getline(stream, var, '"'); // deals with quotations around dir fetched by path()
            if (var.find(".jack") != std::string::npos)
            {
                path << var << std::endl;
                fileCount++; // maintains counter of .vm files
            }
            stream.str("");
        }
    }
    std::cout << path.str() << std::endl; // injested dir for reference
}

int main()
{ // Sends files to tokenizer and vmwriter, while also sets up compilation engine
    analyzer::JackCompiler input;
    do
    {
        std::string fileName;
        std::getline(input.path, fileName); // fetch next file (string pointer mainains state)

        analyzer::JackTokenizer tokenizer(fileName);
        analyzer::VMWriter vmWriter(fileName);
        analyzer::CompilationEngine engine(tokenizer, vmWriter);

        std::cout << fileName << " Compiled successfully" << std::endl;

    } while (--input.fileCount != 0); // for directory

    std::cout << "All files compiled successfully, stored in the same directory" << std::endl;
    return 0;
}
