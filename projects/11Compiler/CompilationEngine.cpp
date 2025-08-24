/* Compilation Engine invokes the tokenizer and in tandem with symbol table and VMWriter, compiles a .jack file into a .vm file
 * Run via JackCompiler.exe

Intended to be run for Jack the object oriented language, created in tandem with  Hack, the custom 16-Bit-PC
*/

#include "JackCompiler.h"
#include <fstream>

namespace
{
    //    std::fstream *handle; // using this as aren't multi-threading(instances)
    int indentLevel = 0;
    void writeData(const std::string &type, std::string data, std::string angleString);
    void indent(int indent);

} // namespace

analyzer::CompilationEngine::CompilationEngine(JackTokenizer &token, VMWriter &VMWriter) : token(token), vmWriter(VMWriter)
{ // dependency injection and begin compilation
    CompileClass();
}

void analyzer::CompilationEngine::CompileClass()
{
    if (token.hasMoreTokens() && std::string(token.current_token) == "class")
    {
        token.hasMoreTokens(); // class, ignore
        token.hasMoreTokens(); // class name
        className = token.current_token;
    }
    else
        vmWriter.Close(); // throw error

    while (token.hasMoreTokens() && std::string(token.current_token) != "}")
    { // ignore { ;compile class or subroutine dec
        if (std::string(token.current_token) == "static" || std::string(token.current_token) == "field")
            CompileClassVarDec();
        else
            CompileSubroutineDec();
    }

    //    writeData("angled-inline", token.current_token, token.tokenType()); // }

    //   indent(-1);
    //  writeData("close", "", "class");
}

void analyzer::CompilationEngine::CompileClassVarDec()
{ // store class var in symbol table

    symbolKind kind = (token.current_token == "static") ? symbolKind::STATIC : symbolKind::FIELD; // static || field
    token.hasMoreTokens();
    std::string type = token.current_token; // type int || char || boolean || *className
    do
    {
        token.hasMoreTokens();
        std::string varName = token.current_token; // var name

        symbolTable.Define(varName, type, kind);
        token.hasMoreTokens(); // , || ;

    } while (token.current_token == ",");
}

void analyzer::CompilationEngine::CompileSubroutineDec()
{ // initalize constructor, function, method

    symbolTable.StartSubroutine();                    // clear subroutine scope
    std::string subroutineType = token.current_token; // constructor || function || method
    if (subroutineType == "method")
        symbolTable.Define("this", className, symbolKind::ARG); // method has implicit this arg

    token.hasMoreTokens();
    std::string type = token.current_token; // type/class name
    token.hasMoreTokens();
    std::string subroutine = token.current_token; // subroutine name
    token.hasMoreTokens();

    CompileParameterList();
    token.hasMoreTokens(); // skip )

    token.hasMoreTokens();                            // skip {
    while (std::string(token.current_token) == "var") // fetch nVars for function label
    {
        CompileVarDec();
        token.hasMoreTokens(); // skip ;
    }
    vmWriter.WriteFunction(className + "." + subroutine, symbolTable.VarCount(symbolKind::VAR)); // function className.functionName nlocals

    if (subroutineType == "constructor") // setup the object
    {
        vmWriter.WritePush(segment::CONST, symbolTable.VarCount(symbolKind::FIELD)); // push const nfield
        vmWriter.WriteCall("Memory.alloc", 1);                                       // call Memory.alloc 1
        vmWriter.WritePop(segment::POINTER, 0);                                      // pop pointer 0, this = base address
    }

    else if (subroutineType == "method") // fetch object of caller
    {
        vmWriter.WritePush(segment::ARG, 0);    // push arg 0
        vmWriter.WritePop(segment::POINTER, 0); // pop pointer 0, this = arg 0
    }

    // no intalization for function
    CompileSubroutineBody();
}

void analyzer::CompilationEngine::CompileParameterList()
{

    token.hasMoreTokens(); // skip (
    while (token.tokenType() == "keyword")
    {
        std::string type = token.current_token; // type
        token.hasMoreTokens();                  // varName
        symbolTable.Define(token.current_token, type, symbolKind::ARG);

        token.hasMoreTokens();
        if (std::string(token.current_token) == ")")
            break;
        token.hasMoreTokens(); // skip ,
    }
}

void analyzer::CompilationEngine::CompileSubroutineBody()
{

    if (std::string(token.current_token) != "}")
    {
        CompileStatements();
    }
    // if (std::string(token.current_token) != "}")   REDUNDANT
    //     std::cout << "error in compilation, current token is " << token.current_token << std::endl;

    // } dealt with in CompileClass
}

void analyzer::CompilationEngine::CompileVarDec()
{ // adds var to symbol table

    token.hasMoreTokens();                  // skip var
    std::string type = token.current_token; // type
    do
    {
        token.hasMoreTokens();                                          // varName
        symbolTable.Define(token.current_token, type, symbolKind::VAR); // add to symbol table
        token.hasMoreTokens();                                          // , || ;
    } while (std::string(token.current_token) == ",");
}

void analyzer::CompilationEngine::CompileStatements()
{ // also fetches next token
    while (1)
    {
        if (std::string(token.current_token) == "let")
            CompileLet();
        else if (std::string(token.current_token) == "if")
            CompileIf();
        else if (std::string(token.current_token) == "while")
            CompileWhile();
        else if (std::string(token.current_token) == "do")
            CompileDo();
        else if (std::string(token.current_token) == "return")
            CompileReturn();
        else
            break;
        token.hasMoreTokens();
    }
}

void analyzer::CompilationEngine::CompileLet()
{ // let varName[expression]? = expression;

    token.hasMoreTokens(); // skip let
    token.hasMoreTokens();
    std::string varName = token.current_token;

    segment seg = (symbolTable.KindOf(varName) == symbolKind::ARG)      ? segment::ARG
                  : (symbolTable.KindOf(varName) == symbolKind::FIELD)  ? segment::THIS
                  : (symbolTable.KindOf(varName) == symbolKind::STATIC) ? segment::STATIC
                  : (symbolTable.KindOf(varName) == symbolKind::VAR)    ? segment::LOCAL
                                                                        : throw std::runtime_error("Undefined variable: " + varName);

    token.hasMoreTokens(); // [ || =

    if (std::string(token.current_token) == "[") // should I do array logic in compileterm / expression or here
    {
        vmWriter.WritePush(seg, symbolTable.IndexOf(varName)); // push array base address
        CompileExpression();
        vmWriter.WriteArithmetic(arithmetic::ADD); // add base address + offset
        token.hasMoreTokens();                     // skip ] CONFIRM if consumed in compileexpression
        token.hasMoreTokens();                     // skip =
        CompileExpression();
        vmWriter.WritePop(segment::TEMP, 0);    // pop expression result to temp 0 (as that pointer may have been changed if e2 had array)
        vmWriter.WritePop(segment::POINTER, 1); // pop base+offset to pointer 1 (that)
        vmWriter.WritePush(segment::TEMP, 0);   // push expression result back to stack
        vmWriter.WritePop(segment::THAT, 0);    // pop expression result to that 0 (that 0 is that base + offset)
    }

    else
    {

        token.hasMoreTokens(); // skip =
        CompileExpression();
        vmWriter.WritePop(seg, symbolTable.IndexOf(varName)); // pop varName
    }

    // ; skipped by compileStatements
}

void analyzer::CompilationEngine::CompileIf()
{
    // expression
    // NOT
    // if-goto l1
    // statement 1
    // goto l2
    //(l1)
    // statement 2
    // (l2)

    token.hasMoreTokens(); // skip (
    token.hasMoreTokens();
    CompileExpression();
    token.hasMoreTokens(); // skip )

    vmWriter.WriteArithmetic(analyzer::arithmetic::NOT); // NOT
    std::string falseLabel = ("IF_FALSE" + std::to_string(labelCount++));
    vmWriter.WriteIf(falseLabel); // if-goto l1
    token.hasMoreTokens();        // skip {
    CompileStatements();          // statement 1
    token.hasMoreTokens();        // skip }

    if (token.hasMoreTokens(1) && std::string(token.current_token) == "else")
    {
        std::string endIf = ("ENDIF" + std::to_string(labelCount++));
        vmWriter.WriteGoto(endIf);       // goto l2
        vmWriter.WriteLabel(falseLabel); // (l1)
        token.hasMoreTokens();
        token.hasMoreTokens();      // skip {
        CompileStatements();        // statement 2
        token.hasMoreTokens();      // skip }
        vmWriter.WriteLabel(endIf); // (l2)
    }
    else
    {
        token.hasMoreTokens(2);          // backtrack (else not present)
        vmWriter.WriteLabel(falseLabel); // (l1)
    }
}

void analyzer::CompilationEngine::CompileWhile()
{
    // label L1
    // compile expression
    // not
    // if goto L2
    // statements
    // goto L1
    // label L2

    std::string whileLabel = ("WHILE" + std::to_string(labelCount++));
    std::string endWhile = ("END_WHILE" + std::to_string(labelCount++));

    token.hasMoreTokens();           // skip while
    vmWriter.WriteLabel(whileLabel); // label L1
    token.hasMoreTokens();           // skip (
    CompileExpression();
    token.hasMoreTokens();                               // skip )
    vmWriter.WriteArithmetic(analyzer::arithmetic::NOT); // not

    vmWriter.Writeif(endWhile);     // if goto L2
    token.hasMoreTokens();          // skip {
    CompileStatements();            // statements
    token.hasMoreTokens();          // skip }
    vmWriter.WriteGoto(whileLabel); // goto L1
    vmWriter.WriteLabel(endWhile);  // label L2
}

void analyzer::CompilationEngine::CompileDo()
{ // do subroutineName(expression) || class/var.subroutineName(expression)

    token.hasMoreTokens();               // skip do
    CompileTerm();                       // handles subroutine call
    token.hasMoreTokens();               // skip ;
    vmWriter.WritePop(segment::TEMP, 0); // discard return value
}

void analyzer::CompilationEngine::CompileReturn()
{ // return expression?;

    token.hasMoreTokens();

    if (std::string(token.current_token) != ";")
        CompileExpression();

    else                                       // void return
        vmWriter.WritePush(segment::CONST, 0); // push 0 for void return

    vmWriter.WriteReturn();
    token.hasMoreTokens(); // skip ;
}

void analyzer::CompilationEngine::CompileExpression()
{ // also fetches next token
    writeData("angled", "", "expression");
    indent(1);

    CompileTerm();
    token.hasMoreTokens();

    char c = token.current_token[0];

    while (c == '+' || c == '-' || c == '*' || c == '/' || c == '&' || c == '|' || c == '=') //<> is handled by &(gt/lt) conversion
    {
        writeData("angled-inline", token.current_token, token.tokenType()); // op
        token.hasMoreTokens();
        CompileTerm();
        token.hasMoreTokens();
        c = token.current_token[0];
    }

    indent(-1);
    writeData("close", "", "expression");
}

void analyzer::CompilationEngine::CompileTerm()
{
    writeData("angled", "", "term");
    indent(1);
    std::string type = token.tokenType();

    if (type == "integerConstant" || type == "stringConstant" || type == "keywordConstant")
        writeData("angled-inline", token.current_token, token.tokenType()); //  term

    else if (std::string(token.current_token) == "(") //(expression)
    {
        writeData("angled-inline", token.current_token, token.tokenType()); //  (
        token.hasMoreTokens();
        CompileExpression();
        writeData("angled-inline", token.current_token, token.tokenType()); //  )
    }

    else if (type == "symbol") // unaryOp term
    {
        writeData("angled-inline", token.current_token, token.tokenType()); //  - | ~
        token.hasMoreTokens();
        CompileTerm();
    }

    else
    {
        writeData("angled-inline", token.current_token, token.tokenType()); //  identifer
        token.hasMoreTokens(1);
        std::string current = std::string(token.current_token);

        if (current == "[") // array
        {
            writeData("angled-inline", token.current_token, token.tokenType()); //  [
            token.hasMoreTokens();
            CompileExpression();
            writeData("angled-inline", token.current_token, token.tokenType()); //  ]
        }
        // subroutine call

        else if (current == "(") //  foo(x)
        {
            writeData("angled-inline", token.current_token, token.tokenType()); //  (
            token.hasMoreTokens();
            if (std::string(token.current_token) != ")")
                CompileExpressionList();
            writeData("angled-inline", token.current_token, token.tokenType()); //  )
        }

        else if (current == ".") // foo.test(x)
        {
            writeData("angled-inline", token.current_token, token.tokenType()); //  .
            token.hasMoreTokens();
            writeData("angled-inline", token.current_token, token.tokenType()); // subroutineName
            token.hasMoreTokens();
            writeData("angled-inline", token.current_token, token.tokenType()); // (
            token.hasMoreTokens();
            if (std::string(token.current_token) != ")")
                CompileExpressionList();
            writeData("angled-inline", token.current_token, token.tokenType()); //  )
        }

        else
        {
            token.hasMoreTokens(2); // backtrack
        }
    }

    indent(-1);
    writeData("close", "", "term");
}

void analyzer::CompilationEngine::CompileExpressionList()
{
    writeData("angled", "", "expressionList");
    indent(1);

    CompileExpression();
    // token.hasMoreTokens(1);
    while (std::string(token.current_token) == ",")
    {
        writeData("angled-inline", token.current_token, token.tokenType()); //  ,
        token.hasMoreTokens();
        CompileExpression();
    }
    // token.hasMoreTokens(2); // backtrack (redundant as step fwd not done againj)

    indent(-1);
    writeData("close", "", "expressionList");
}

namespace
{
    void writeData(const std::string &type, std::string data, std::string angleString)
    {
        indent(0);
        if (type == "angled")
            *handle << '<' + angleString + ">\n";

        else if (type == "angled-inline") // inline angled or similar change?
        {
            *handle << '<' + angleString + ">";
            *handle << ' ' + data + ' ';
            *handle << "</" + angleString + ">\n";
        }
        else if (type == "close")
            *handle << "</" + angleString + ">\n";

        else
            *handle << data + '\n';
    }

    void indent(int indent) // 1 to increment, -1 to decrement, 0 to write
    {
        if (indent == 1)
            indentLevel++;
        else if (indent == -1)
            indentLevel--;
        else
        {

            for (int i = indentLevel; i > 0; i--)
            {
                *handle << "  ";
            }
        }
    }
}
