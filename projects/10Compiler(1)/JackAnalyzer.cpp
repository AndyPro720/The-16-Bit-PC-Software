/* VMTranslator takes input a Jack language file / directory of files and provides parsed/analyzed output in XML files

Intended to be run for Hack, the custom 16-Bit-PC
*/

#include <filesystem>
#include "JackAnalyzer.h"

analyzer::JackAnalyzer::JackAnalyzer()
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
{
    analyzer::JackAnalyzer input;
    do
    {
        analyzer::JackTokenizer token(input.path);
        token.hasMoreTokens();

    } while (--input.fileCount != 0); // if directory

    return 0;
}
