/* VMWriter.cpp is invoked by the CompilationEngine to write VM commands into a .vm file.
    Run via JackCompiler.exe

Intended to be run for Jack the object oriented language, created in tandem with Hack, the custom 16-Bit-PC
*/

#include "JackCompiler.h"

analyzer::VMWriter::VMWriter(const std::string &inputFile)
{ // creates output file for writing VM commands

    std::string outputFile = inputFile;
    outputFile.resize(outputFile.find('.'));

    fhandle.open(outputFile + "_c" + ".vm", std::ofstream::out | std::ofstream::trunc);
}

void analyzer::VMWriter::WritePush(segment seg, int index)
{ // push constant 10
    std::string segment;
    (seg == segment::LOCAL) ? segment = "local" : (seg == segment::CONST) ? segment = "constant"
                                              : (seg == segment::ARG)     ? segment = "argument"
                                              : (seg == segment::STATIC)  ? segment = "static"
                                              : (seg == segment::THIS)    ? segment = "this"
                                              : (seg == segment::THAT)    ? segment = "that"
                                              : (seg == segment::POINTER) ? segment = "pointer"
                                              : (seg == segment::TEMP)    ? segment = "temp"
                                                                          : segment = "";
    fhandle << "push " << segment << " " << index << std::endl;
}

void analyzer::VMWriter::WritePop(segment seg, int index)
{ // pop local 0
    std::string segment;
    (seg == segment::LOCAL) ? segment = "local" : (seg == segment::ARG)   ? segment = "argument"
                                              : (seg == segment::STATIC)  ? segment = "static"
                                              : (seg == segment::THIS)    ? segment = "this"
                                              : (seg == segment::THAT)    ? segment = "that"
                                              : (seg == segment::POINTER) ? segment = "pointer"
                                              : (seg == segment::TEMP)    ? segment = "temp"
                                                                          : segment = "";
    fhandle << "pop " << segment << " " << index << std::endl;
}

void analyzer::VMWriter::WriteArithmetic(arithmetic command)
{ // add (sub, neg, eq...)
    std::string command_str;
    (command == arithmetic::ADD) ? command_str = "add" : (command == arithmetic::SUB) ? command_str = "sub"
                                                     : (command == arithmetic::NEG)   ? command_str = "neg"
                                                     : (command == arithmetic::EQ)    ? command_str = "eq"
                                                     : (command == arithmetic::GT)    ? command_str = "gt"
                                                     : (command == arithmetic::LT)    ? command_str = "lt"
                                                     : (command == arithmetic::AND)   ? command_str = "and"
                                                     : (command == arithmetic::OR)    ? command_str = "or"
                                                     : (command == arithmetic::NOT)   ? command_str = "not"
                                                                                      : command_str = "";
    fhandle << command_str << std::endl;
}

void analyzer::VMWriter::WriteLabel(const std::string &label)
{ // label LOOP_START
    fhandle << "label " << label << std::endl;
}

void analyzer::VMWriter::WriteGoto(const std::string &label)
{ // goto LOOP_START
    fhandle << "goto " << label << std::endl;
}

void analyzer::VMWriter::WriteIf(const std::string &label)
{ // if-goto LOOP_START
    fhandle << "if-goto " << label << std::endl;
}

void analyzer::VMWriter::WriteCall(const std::string &name, int nArgs)
{ // call functionName nArgs
    fhandle << "call " << name << " " << nArgs << std::endl;
}

void analyzer::VMWriter::WriteFunction(const std::string &name, int nLocals)
{ // function functionName nLocals
    fhandle << "function " << name << " " << nLocals << std::endl;
}

void analyzer::VMWriter::WriteReturn()
{ // return
    fhandle << "return" << std::endl;
}

void analyzer::VMWriter::Close()
{
    fhandle.close();
}