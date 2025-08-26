/* Compilation Engine invokes the tokenizer and in tandem with symbol table and VMWriter, compiles a .jack file into a .vm file
 * Run via JackCompiler.exe

Intended to be run for Jack the object oriented language, created in tandem with  Hack, the custom 16-Bit-PC
*/

#include "JackCompiler.h"
#include <fstream>

analyzer::CompilationEngine::CompilationEngine(JackTokenizer &token, VMWriter &VMWriter) : token(token), vmWriter(VMWriter)
{ // dependency injection and begin compilation
    labelCount = 0;
    CompileClass();
}

void analyzer::CompilationEngine::CompileClass()
{
    if (token.hasMoreTokens() && std::string(token.current_token) == "class")
    {
        token.hasMoreTokens(); // class, ignore
        className = token.current_token;
    }
    else
    {
        throw std::runtime_error("Expected 'class' keyword at start of class");
        vmWriter.Close();
    }

    token.hasMoreTokens(); // skip {
    while (token.hasMoreTokens() && std::string(token.current_token) != "}")
    { // compile class or subroutine dec
        if (std::string(token.current_token) == "static" || std::string(token.current_token) == "field")
            CompileClassVarDec();
        else
            CompileSubroutineDec();
    }
    vmWriter.Close(); // close file handle
}

void analyzer::CompilationEngine::CompileClassVarDec()
{ // store class var in symbol table

    symbolKind kind = (std::string(token.current_token) == "static") ? symbolKind::STATIC : symbolKind::FIELD; // static || field
    token.hasMoreTokens();

    std::string type = token.current_token; // type int || char || boolean || *className
    do
    {
        token.hasMoreTokens();
        std::string varName = token.current_token; // var name

        symbolTable.Define(varName, type, kind);
        token.hasMoreTokens(); // , || ;

    } while (std::string(token.current_token) == ",");
    token.hasMoreTokens(); // skip ;
}

void analyzer::CompilationEngine::CompileSubroutineDec()
{ // initalize constructor, function, method

    symbolTable.StartSubroutine();                    // clear subroutine scope
    std::string subroutineType = token.current_token; // constructor || function || method
    if (subroutineType == "method")
        symbolTable.Define("this", className, symbolKind::ARG); // method has implicit this arg

    token.hasMoreTokens();
    std::string type = token.current_token; // return_type/class/void
    token.hasMoreTokens();
    std::string subroutine = token.current_token; // subroutine name
    token.hasMoreTokens();

    CompileParameterList();
    token.hasMoreTokens(); // skip )

    token.hasMoreTokens();                            // skip {
    while (std::string(token.current_token) == "var") // fetch nVars for function label
    {
        CompileVarDec();
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

    // no seperate intalization for function
    CompileSubroutineBody();
    token.hasMoreTokens(); // skip } for subroutine
}

void analyzer::CompilationEngine::CompileParameterList()
{
    token.hasMoreTokens(); // skip (
    while (std::string(token.current_token) != ")")
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
    token.hasMoreTokens(); // skip ;
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

    if (std::string(token.current_token) == "[")
    {
        token.hasMoreTokens();                                 // skip [
        vmWriter.WritePush(seg, symbolTable.IndexOf(varName)); // push array base address
        CompileExpression();
        vmWriter.WriteArithmetic(arithmetic::ADD); // add base address + offset
        token.hasMoreTokens();                     // skip ]
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
    token.hasMoreTokens(); // skip ;
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

    if (token.hasMoreTokens() && std::string(token.current_token) == "else")
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
        // already on next token
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

    vmWriter.WriteIf(endWhile);     // if goto L2
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
{ // term (op term)*

    CompileTerm();

    char op = token.current_token[0];

    while (op == '+' || op == '-' || op == '*' || op == '/' || op == '&' || op == '|' || op == '=' || op == '<' || op == '>')
    {

        token.hasMoreTokens(); // term
        CompileTerm();

        (op == '+')   ? vmWriter.WriteArithmetic(arithmetic::ADD)
        : (op == '-') ? vmWriter.WriteArithmetic(arithmetic::SUB)
        : (op == '*') ? vmWriter.WriteCall("Math.multiply", 2)
        : (op == '/') ? vmWriter.WriteCall("Math.divide", 2)
        : (op == '&') ? vmWriter.WriteArithmetic(arithmetic::AND)
        : (op == '|') ? vmWriter.WriteArithmetic(arithmetic::OR)
        : (op == '=') ? vmWriter.WriteArithmetic(arithmetic::EQ)
        : (op == '<') ? vmWriter.WriteArithmetic(arithmetic::LT)
        : (op == '>') ? vmWriter.WriteArithmetic(arithmetic::GT)
                      : throw std::runtime_error("Unknown operator" + std::string(1, op));

        op = token.current_token[0]; // compileterm auto advances to next token
    }
}

void analyzer::CompilationEngine::CompileTerm()
{
    std::string type = token.tokenType();

    if (type == "integerConstant")
    {
        vmWriter.WritePush(segment::CONST, std::stoi(token.current_token)); // push constant int
        token.hasMoreTokens();
    }

    else if (type == "stringConstant")
    {
        std::string str = token.current_token;
        vmWriter.WritePush(segment::CONST, str.length()); // push constant length
        vmWriter.WriteCall("String.new", 1);              // String.new length

        for (char c : str)
        {
            vmWriter.WritePush(segment::CONST, c);      // push constant char
            vmWriter.WriteCall("String.appendChar", 2); // String.appendChar  this + char
        }
        token.hasMoreTokens();
    }

    else if (type == "keywordConstant") // true, false, null, this
    {
        if (std::string(token.current_token) == "true")
        {
            vmWriter.WritePush(segment::CONST, 0);     // push 0
            vmWriter.WriteArithmetic(arithmetic::NOT); // NOT to get -1 (all bits high)
        }
        else if (std::string(token.current_token) == "false" || std::string(token.current_token) == "null")
            vmWriter.WritePush(segment::CONST, 0); // push 0
        else if (std::string(token.current_token) == "this")
            vmWriter.WritePush(segment::POINTER, 0); // push pointer 0 (this)

        token.hasMoreTokens();
    }

    else if (std::string(token.current_token) == "(") //(expression)
    {
        token.hasMoreTokens(); // skip (
        CompileExpression();
        token.hasMoreTokens(); // skip )
    }

    else if (type == "symbol" && (std::string(token.current_token) == "-" || std::string(token.current_token) == "~")) // redundant check for symbols
    {
        std::string op = token.current_token;
        token.hasMoreTokens(); // skip unaryOp
        CompileTerm();

        if (op == "-")
            vmWriter.WriteArithmetic(arithmetic::NEG);
        else if (op == "~") // simple else works too
            vmWriter.WriteArithmetic(arithmetic::NOT);
        else
            throw std::runtime_error("Unknown unary operator: " + std::string(token.current_token));
    }

    else
    {
        std::string identifier = token.current_token;
        token.hasMoreTokens();
        std::string peek = token.current_token;

        if (peek == "[") // array
        {
            token.hasMoreTokens();
            segment seg = (symbolTable.KindOf(identifier) == symbolKind::ARG)      ? segment::ARG
                          : (symbolTable.KindOf(identifier) == symbolKind::FIELD)  ? segment::THIS
                          : (symbolTable.KindOf(identifier) == symbolKind::STATIC) ? segment::STATIC
                          : (symbolTable.KindOf(identifier) == symbolKind::VAR)    ? segment::LOCAL
                                                                                   : throw std::runtime_error("Undefined variable: " + identifier);
            vmWriter.WritePush(seg, symbolTable.IndexOf(identifier)); // push array base address
            CompileExpression();
            vmWriter.WriteArithmetic(arithmetic::ADD); // add base address + offset
            vmWriter.WritePop(segment::POINTER, 1);    // set that to base+offset
            vmWriter.WritePush(segment::THAT, 0);      // access (*that) and push to stack
            token.hasMoreTokens();                     // skip ]
        }

        // subroutine call

        else if (peek == "(") // method call  foo(x)
        {
            int nArgs = 0;
            token.hasMoreTokens();                   // consume (
            vmWriter.WritePush(segment::POINTER, 0); // push this as current object
            if (std::string(token.current_token) != ")")
                nArgs = CompileExpressionList();
            token.hasMoreTokens(); // consume )

            vmWriter.WriteCall(identifier, nArgs + 1); // call functionName nArgs + this
        }

        else if (peek == ".") // foo.test(x)
        {
            // class.foo = function or constructor
            //  var.foo = method

            int nArgs = 0;
            if (symbolTable.KindOf(identifier) == symbolKind::NONE) // function or constructor
            {
                token.hasMoreTokens(); // skip .
                identifier = identifier + "." + token.current_token;

                token.hasMoreTokens(); // skip (
                if (std::string(token.current_token) != ")")
                    nArgs = CompileExpressionList();
                token.hasMoreTokens(); // skip )
            }

            else // method
            {
                segment seg = (symbolTable.KindOf(identifier) == symbolKind::ARG)      ? segment::ARG
                              : (symbolTable.KindOf(identifier) == symbolKind::FIELD)  ? segment::THIS
                              : (symbolTable.KindOf(identifier) == symbolKind::STATIC) ? segment::STATIC
                              : (symbolTable.KindOf(identifier) == symbolKind::VAR)    ? segment::LOCAL
                                                                                       : throw std::runtime_error("Undefined variable: " + identifier);
                vmWriter.WritePush(seg, symbolTable.IndexOf(identifier)); // push var as current object aka this
                token.hasMoreTokens();                                    // skip .
                identifier = symbolTable.TypeOf(identifier) + "." + token.current_token;

                token.hasMoreTokens(); // skip (
                if (std::string(token.current_token) != ")")
                    nArgs = CompileExpressionList();
                nArgs++;               // +1 for this
                token.hasMoreTokens(); // skip )
            }
            vmWriter.WriteCall(identifier, nArgs); // call functionName nArgs
        }

        else // var name
        {
            symbolKind kind = symbolTable.KindOf(identifier);
            segment seg = (kind == symbolKind::ARG)      ? segment::ARG
                          : (kind == symbolKind::FIELD)  ? segment::THIS
                          : (kind == symbolKind::STATIC) ? segment::STATIC
                          : (kind == symbolKind::VAR)    ? segment::LOCAL
                                                         : throw std::runtime_error("Undefined variable: " + identifier);
            vmWriter.WritePush(seg, symbolTable.IndexOf(identifier)); // push varName
                                                                      // no need to adv token as already peeked
        }
    }
}

int analyzer::CompilationEngine::CompileExpressionList()
{ // (expression (, expression)*)?

    if (std::string(token.current_token) == ")") // redundant check
        return 0;

    int nArgs = 1;
    CompileExpression();
    while (std::string(token.current_token) == ",")
    {
        nArgs++;
        token.hasMoreTokens(); // skip ,
        CompileExpression();
    }
    return nArgs;
}
