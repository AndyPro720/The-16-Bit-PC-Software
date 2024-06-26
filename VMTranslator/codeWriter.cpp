#include <iostream>
#include <fstream>
#include "VMTranslator.h"

write::codeWriter::codeWriter(std::string file)
{
    if (filename.find(".vm") != std::string::npos)
    {
        log.open(file + "_log_file.txt", std::ostream::out | std::ostream::binary | std::ofstream::trunc);
        o_file_handle.open(file + ".asm", std::ofstream::out | std::ostream::binary | std::ofstream::trunc);
    }
    else // if dir, put output within dir
    {
        log.open(file + "\\" + file + "_log_file.txt", std::ostream::out | std::ostream::binary | std::ofstream::trunc);
        o_file_handle.open(file + "\\" + file + ".asm", std::ofstream::out | std::ostream::binary | std::ofstream::trunc);
    }

    // write init code
    std::string initCode;
    initCode = std::string("@256\nD=A\n@SP\nM=D\n");

    log << "//Bootstrap Code\n"
        << initCode;
    o_file_handle << initCode;

    writeCall("Sys.init", 0); // call Sys.init
}

void write::codeWriter::writeArithmetic(std::string type)
{
    std::string command;
    std::string arith_template = std::string("@SP\n") + // Puts the value of stack in D and M registers respectively
                                 "AM=M-1\n" +
                                 "D=M\n" +
                                 "A=A-1\n";
    if (type == "add" || type == "sub" || type == "and" || type == "or")
    {
        std::string symbol;
        (type == "add") ? symbol = "+" : (type == "sub") ? symbol = "-"
                                     : (type == "and")   ? symbol = "&"
                                     : (type == "or")    ? symbol = "|"
                                                         : symbol = "";

        command = arith_template + "M=M" + symbol + "D\n";
    }

    else if (type == "neg" || type == "not")
    {
        std::string symbol;
        (type == "neg") ? symbol = "-" : (type == "not") ? symbol = "!"
                                                         : symbol = "";
        command = std::string("@SP\n") +
                  "A=M-1\n" +
                  "M=" + symbol + "M\n";
    }

    else if (type == "eq" || type == "gt" || type == "lt") // return -1 if true and 0 if false
    {
        std::string symbol;
        (type == "eq") ? symbol = ";JEQ\n" : (type == "gt") ? symbol = ";JGT\n"
                                         : (type == "lt")   ? symbol = ";JLT\n"
                                                            : symbol = "";

        command = arith_template +
                  "D=M-D\n" + // store diff
                  "@TRUE." + std::to_string(count) + '\n' +
                  "D" + symbol +

                  "@SP\n" // if false
                  "A=M-1\n" +
                  "M=0\n" +
                  "@NEXT." + std::to_string(count) + '\n' +
                  "0;JMP\n" +

                  "(TRUE." + std::to_string(count) + ")\n" + // if true
                  "@SP\n"
                  "A=M-1\n" +
                  "M=-1\n" +
                  "(NEXT." + std::to_string(count) + ")\n";
        count++; // to keep each label unique
    }

    o_file_handle << command;
    log << "\n// " + type + '\n' + command;
}

void write::codeWriter::writePushPop(std::string type, std::string segment, int index, std::string current_file)
{
    if (type == "C_PUSH")
    {
        std::string command;
        std::string push_template = std::string("@SP\n") + // pushes value to stack and increments stack
                                    "A=M\n" +
                                    "M=D\n" +
                                    "@SP\n" +
                                    "M=M+1\n";
        if (segment == "constant")
        {
            command = '@' + std::to_string(index) + '\n' +
                      "D=A\n" +
                      push_template;
        }

        else if (segment == "local" || segment == "argument" || segment == "this" || segment == "that")
        {
            std::string seg;
            (segment == "local") ? seg = "LCL" : (segment == "argument") ? seg = "ARG"
                                             : (segment == "this")       ? seg = "THIS"
                                             : (segment == "that")       ? seg = "THAT"
                                                                         : seg = "";

            command = "@" + std::to_string(index) + '\n' +
                      "D=A\n" +
                      "@" + seg + '\n' +
                      "A=M+D\n" +
                      "D=M\n" +
                      push_template;
        }

        else if (segment == "static")
        {
            command = '@' + current_file + '.' + std::to_string(index) + '\n' +
                      "D=M\n" +
                      push_template;
        }

        else if (segment == "temp")
        {
            if (index > 7)
            {
                std::cout << "Temp index " << index << "is out of scope";
                close(1);
            }
            command = '@' + std::to_string(5 + index) + '\n' +
                      "D=M\n" +
                      push_template;
        }

        else if (segment == "pointer")
        {
            if (!(index == 0 || index == 1))
            {
                std::cout << "Only Pointer 0/1 is allowed";
                close(1);
            }
            if (index == 0)
            {
                command = std::string("@THIS\n") +
                          "D=M\n" +
                          push_template;
            }
            else if (index == 1)
            {
                command = std::string("@THAT\n") +
                          "D=M\n" +
                          push_template;
            }
        }

        // write to file
        o_file_handle << command;
        log << "\n//PUSH " + segment + ' ' + std::to_string(index) + '\n' + command;
    }

    else // if C_POP
    {
        std::string command;
        std::string pop_template = std::string("@SP\n") + // pops value from stack and decrements sp
                                   "AM=M-1\n" +
                                   "D=M\n";

        if (segment == "local" || segment == "argument" || segment == "this" || segment == "that")
        {
            std::string seg;
            (segment == "local") ? seg = "LCL" : (segment == "argument") ? seg = "ARG"
                                             : (segment == "this")       ? seg = "THIS"
                                             : (segment == "that")       ? seg = "THAT"
                                                                         : seg = "";

            if (index == 0) // fetch value from stack and simply push to segment
            {
                command = pop_template +
                          "@" + seg + '\n' +
                          "A=M\n" +
                          "M=D\n";
            }
            else // fetch value from stack and push to segment offset
            {
                command = "@" + std::to_string(index) + '\n' +
                          "D=A\n" +
                          "@" + seg + '\n' +
                          "D=D+M\n" +
                          "@R13\n" +
                          "M=D\n" +      // store segment pointer with offset in R13
                          pop_template + // pop value from stack
                          "@R13\n"
                          "A=M\n"
                          "M=D\n"; // pop value in segment pointer
            }
        }

        else if (segment == "static")
        {
            command = pop_template +
                      '@' + current_file + '.' + std::to_string(index) + '\n' +
                      "M=D\n";
        }

        else if (segment == "temp")
        {
            if (index > 7)
            {
                std::cout << "Temp index " << index << "is out of scope";
                close(1);
            }
            command = pop_template +
                      '@' + std::to_string(5 + index) + '\n' +
                      "M=D\n";
        }

        else if (segment == "pointer")
        {
            if (!(index == 0 || index == 1))
            {
                std::cout << "Only Pointer 0/1 is allowed";
                close(1);
            }
            if (index == 0)
            {
                command = pop_template +
                          std::string("@THIS\n") +
                          "M=D\n";
            }
            else if (index == 1)
            {
                command = pop_template +
                          std::string("@THAT\n") +
                          "M=D\n";
            }
        }

        // write to file
        o_file_handle << command;
        log << "\n//POP " + segment + ' ' + std::to_string(index) + '\n' + command;
    }
}

void write::codeWriter::writeLabel(std::string label)
{ // create a label
    std::string command = '(' + label + ')' + '\n';
    o_file_handle << command;
    log << "\n//Label " + label + '\n' + command;
}

void write::codeWriter::writeGoto(std::string label)
{ // unconditional jump to label
    std::string command = '@' + label + '\n' + "0;JMP\n";
    o_file_handle << command;
    log << "\n//goto " + label + '\n' + command;
}

void write::codeWriter::writeIf(std::string label)
{ // pops value from stack and jumps to label if value != 0
    std::string command = std::string("@SP\n") + "AM=M-1\n" + "D=M\n" + '@' + label + '\n' + "D;JNE\n";
    o_file_handle << command;
    log << "\n//if-goto " + label + '\n' + command;
}

void write::codeWriter::writeFunction(std::string functionName, int numVars)
{ // Declares the function and intializes local variables
    std::string command = '(' + functionName + ")\n";
    o_file_handle << command;
    log << "\n //function " + functionName + ' ' + std::to_string(numVars) + '\n' + command;
    while (numVars != 0)
    {
        writePushPop("C_PUSH", "constant", 0, "");
        numVars--;
    }
}

void write::codeWriter::writeCall(std::string functionName, int numArgs)
{ // saves return address, caller's stack (frame), adjusts ARG and LCL pointers and jumps to called function

    std::string push_template = std::string("@SP\n") + // pushes value to stack and increments stack
                                "A=M\n" +
                                "M=D\n" +
                                "@SP\n" +
                                "M=M+1\n";
    std::string command =
        "@" + functionName + '.' + std::to_string(returnCount) + '\n' + "D=A\n" + push_template + // push return address to stack
        "@LCL\n" + "D=M\n" + push_template +                                                      // save frame
        "@ARG\n" + "D=M\n" + push_template +
        "@THIS\n" + "D=M\n" + push_template +
        "@THAT\n" + "D=M\n" + push_template +
        "@" + std::to_string(5 + numArgs) + '\n' + "D=A\n" + "@SP\n" + "D=M-D\n" +
        "@ARG\n" + "M=D\n" +                    // ARG = SP-5-numArgs
        "@SP\n" + "D=M\n" + "@LCL\n" + "M=D\n"; // LCL = SP

    o_file_handle << command;
    log << "\n//Call" + functionName + std::to_string(numArgs) + " (push return add, save frame, repoint ARG and LCL) \n" + command;

    writeGoto(functionName);

    o_file_handle << "(" + functionName + '.' + std::to_string(returnCount) + ")\n";
    log << "\n //Return Address Label\n" + std::string("(") + functionName + '.' + std::to_string(returnCount) + ")\n";

    returnCount++;
}

void write::codeWriter::Return()
{ // returns function value, restores stack to caller's frame, and moves execution to after function call
    std::string command =
        std::string("@LCL\n") + "D=M\n" + "@endFrame\n" + "M=D\n" +                    // endFrame = LCL
        "@5\n" + "A=D-A\n" + "D=M\n" + "@retAdd\n" + "M=D\n" +                         // retAdd = *(endframe - 5)
        "@SP\n" + "AM=M-1\n" + "D=M\n" + "@ARG\n" + "A=M\n" + "M=D\n" +                //*ARG = pop(return value)
        "@ARG\n" + "D=M+1\n" + "@SP\n" + "M=D\n" +                                     // SP = ARG+1
        "@endFrame\n" + "A=M-1\n" + "D=M\n" + "@THAT\n" + "M=D\n" +                    // THAT = *(endFrame -1)
        "@2\n" + "D=A\n" + "@endFrame\n" + "A=M-D\n" + "D=M\n" + "@THIS\n" + "M=D\n" + // THIS = *(endFrame -2)
        "@3\n" + "D=A\n" + "@endFrame\n" + "A=M-D\n" + "D=M\n" + "@ARG\n" + "M=D\n" +  // ARG = *(endFrame -3)
        "@4\n" + "D=A\n" + "@endFrame\n" + "A=M-D\n" + "D=M\n" + "@LCL\n" + "M=D\n" +  // LCL = *(endFrame -4)
        "@retAdd\n" + "A=M\n" + "0;JMP\n";                                             // goto return address

    o_file_handle << command;
    log << "\n//Return \n" + command;
}

int write::codeWriter::close(bool flag) // close the files or drop translation
{
    if (flag)
    {
        std::cout << "\nError found, terminating translation.\nCheck command after last sucessful translation in log\n";
        o_file_handle.close();
        log.close();
        exit(1);
    }
    else
    {
        std::string end_loop;
        end_loop = "(END)\n@END\n0;JMP";

        o_file_handle << end_loop;
        log << "\n//END LOOP\n" + end_loop;

        o_file_handle.close();
        log.close();
        std::cout << "File and log_file translated with same name successfully! \n";
        return 0;
    }
}