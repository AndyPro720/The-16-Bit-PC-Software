/* Compilation Engine invokes the tokenizer and creates a processed xml parse tree from all the tokens, output as filename_c.xml
 * Run via JackAnalyzer.exe

Intended to be run for Jack the object oriented language, created in tandem with  Hack, the custom 16-Bit-PC
*/

#include "JackAnalyzer.h"
#include <fstream>

namespace
{
    std::fstream *handle; // using this as aren't multi-threading(instances)
    int indentLevel = 0;
    void writeData(const std::string &type, std::string data, std::string angleString);
    void indent(int indent);

} // namespace

analyzer::CompilationEngine::CompilationEngine(std::stringstream &path) : token(path)
{
    std::string filename = token.filename_g;

    filename.resize(filename.find('.'));
    filehandle.open(filename + "_c" + ".xml", std::ofstream::out | std::ofstream::trunc);
    handle = &filehandle;

    CompileClass();
}

void analyzer::CompilationEngine::CompileClass()
{
    if (token.hasMoreTokens() && std::string(token.current_token) == "class")
    {

        writeData("angled", " ", token.current_token);
        indent(1);
        writeData("angled-inline", token.current_token, token.tokenType());
        token.hasMoreTokens(); //  class name
        writeData("angled-inline", token.current_token, token.tokenType());
        token.hasMoreTokens(); //  {
        writeData("angled-inline", token.current_token, token.tokenType());
    }
    else
        Close(0); // throw error

    while (token.hasMoreTokens() && std::string(token.current_token) != "}")
    {
        if (std::string(token.current_token) == "static" || std::string(token.current_token) == "field")
            CompileClassVarDec();
        else
            CompileSubroutineDec();
    }

    writeData("angled-inline", token.current_token, token.tokenType()); // }

    indent(-1);
    writeData("close", "", "class");
}

void analyzer::CompilationEngine::CompileClassVarDec()
{

    writeData("angled", "", "classVarDec");
    indent(1);
    writeData("angled-inline", token.current_token, token.tokenType()); // static || field
    token.hasMoreTokens();
    writeData("angled-inline", token.current_token, token.tokenType()); // type
    token.hasMoreTokens();
    writeData("angled-inline", token.current_token, token.tokenType()); // var name

    while (token.hasMoreTokens() && std::string(token.current_token) == ",")
    {
        writeData("angled-inline", token.current_token, token.tokenType()); // ,
        token.hasMoreTokens();
        writeData("angled-inline", token.current_token, token.tokenType()); //  var name
    }

    writeData("angled-inline", token.current_token, token.tokenType()); // ;
    indent(-1);
    writeData("close", "", "classVarDec");
}

void analyzer::CompilationEngine::CompileSubroutineDec()
{
    writeData("angled", "", "subroutineDec");
    indent(1);
    writeData("angled-inline", token.current_token, token.tokenType()); // constructor || function || method
    token.hasMoreTokens();
    writeData("angled-inline", token.current_token, token.tokenType()); // void || type
    token.hasMoreTokens();
    writeData("angled-inline", token.current_token, token.tokenType()); // name
    token.hasMoreTokens();
    writeData("angled-inline", token.current_token, token.tokenType()); // (

    CompileParameterList();
    if (std::string(token.current_token) != ")")
        token.hasMoreTokens();

    writeData("angled-inline", token.current_token, token.tokenType()); // )

    token.hasMoreTokens();
    CompileSubroutineBody();

    indent(-1);
    writeData("close", "", "subroutineDec");
}

void analyzer::CompilationEngine::CompileParameterList()
{

    writeData("angled", "", "parameterList");
    indent(1);
    token.hasMoreTokens();
    while (token.tokenType() == "keyword")
    {
        writeData("angled-inline", token.current_token, token.tokenType()); // type
        token.hasMoreTokens();
        writeData("angled-inline", token.current_token, token.tokenType()); // name
        token.hasMoreTokens();
        if (std::string(token.current_token) == ")")
            break;
        writeData("angled-inline", token.current_token, token.tokenType()); // ,
        token.hasMoreTokens();
    }
    indent(-1);
    writeData("close", "", "parameterList");
}

void analyzer::CompilationEngine::CompileSubroutineBody()
{
    writeData("angled", "", "subroutineBody");
    indent(1);
    writeData("angled-inline", token.current_token, token.tokenType()); // {
    token.hasMoreTokens();
    while (std::string(token.current_token) == "var")
    {
        CompileVarDec();
        token.hasMoreTokens();
    }

    if (std::string(token.current_token) != "}")
    {
        CompileStatements();
    }
    if (std::string(token.current_token) != "}")
        std::cout << "error in compilation, current token is " << token.current_token << std::endl;

    writeData("angled-inline", token.current_token, token.tokenType()); // }
    indent(-1);
    writeData("close", "", "subroutineBody");
}

void analyzer::CompilationEngine::CompileVarDec()
{

    writeData("angled", "", "varDec");
    indent(1);
    writeData("angled-inline", token.current_token, token.tokenType()); // var
    token.hasMoreTokens();
    writeData("angled-inline", token.current_token, token.tokenType()); // type
    token.hasMoreTokens();
    writeData("angled-inline", token.current_token, token.tokenType()); // varName
    token.hasMoreTokens();
    while (std::string(token.current_token) == ",")
    {
        writeData("angled-inline", token.current_token, token.tokenType()); // ,
        token.hasMoreTokens();
        writeData("angled-inline", token.current_token, token.tokenType()); // varName
        token.hasMoreTokens();
    }
    writeData("angled-inline", token.current_token, token.tokenType()); // ;
    indent(-1);
    writeData("close", "", "varDec");
}

void analyzer::CompilationEngine::CompileStatements()
{ // also fetches next token
    writeData("angled", "", "statements");
    indent(1);

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
    indent(-1);
    writeData("close", "", "statements");
}

void analyzer::CompilationEngine::CompileLet()
{
    writeData("angled", "", "letStatement");
    indent(1);

    writeData("angled-inline", token.current_token, token.tokenType()); // let
    token.hasMoreTokens();
    writeData("angled-inline", token.current_token, token.tokenType()); // varName
    token.hasMoreTokens();

    if (std::string(token.current_token) == "[")
    {
        writeData("angled-inline", token.current_token, token.tokenType()); // [
        token.hasMoreTokens();
        CompileExpression();
        writeData("angled-inline", token.current_token, token.tokenType()); // ]
        token.hasMoreTokens();
    }

    writeData("angled-inline", token.current_token, token.tokenType()); // =
    token.hasMoreTokens();
    CompileExpression();
    writeData("angled-inline", token.current_token, token.tokenType()); // ;

    indent(-1);
    writeData("close", "", "letStatement");
}

void analyzer::CompilationEngine::CompileIf()
{
    writeData("angled", "", "ifStatement");
    indent(1);

    writeData("angled-inline", token.current_token, token.tokenType()); // if
    token.hasMoreTokens();
    writeData("angled-inline", token.current_token, token.tokenType()); // (
    token.hasMoreTokens();
    CompileExpression();
    writeData("angled-inline", token.current_token, token.tokenType()); // )
    token.hasMoreTokens();
    writeData("angled-inline", token.current_token, token.tokenType()); // {
    token.hasMoreTokens();
    CompileStatements();
    writeData("angled-inline", token.current_token, token.tokenType()); // }

    if (token.hasMoreTokens(1) && std::string(token.current_token) == "else")
    {
        writeData("angled-inline", token.current_token, token.tokenType()); // else
        token.hasMoreTokens();
        writeData("angled-inline", token.current_token, token.tokenType()); // {
        token.hasMoreTokens();
        CompileStatements();
        writeData("angled-inline", token.current_token, token.tokenType()); // }
    }
    else
        token.hasMoreTokens(2);

    indent(-1);
    writeData("close", "", "ifStatement");
}

void analyzer::CompilationEngine::CompileWhile()
{
    writeData("angled", "", "whileStatement");
    indent(1);

    writeData("angled-inline", token.current_token, token.tokenType()); // while
    token.hasMoreTokens();
    writeData("angled-inline", token.current_token, token.tokenType()); // (
    token.hasMoreTokens();
    CompileExpression();
    writeData("angled-inline", token.current_token, token.tokenType()); // )
    token.hasMoreTokens();
    writeData("angled-inline", token.current_token, token.tokenType()); // {
    token.hasMoreTokens();
    CompileStatements();
    writeData("angled-inline", token.current_token, token.tokenType()); // }

    indent(-1);
    writeData("close", "", "whileStatement");
}

void analyzer::CompilationEngine::CompileDo()
{
    writeData("angled", "", "doStatement");
    indent(1);

    writeData("angled-inline", token.current_token, token.tokenType()); // do
    token.hasMoreTokens();
    writeData("angled-inline", token.current_token, token.tokenType()); // subroutineName || class || var name
    token.hasMoreTokens();
    // subroutineCall
    if (std::string(token.current_token) == "(")
    {
        writeData("angled-inline", token.current_token, token.tokenType()); // (
        token.hasMoreTokens();
        if (std::string(token.current_token) != ")")
            CompileExpressionList();
        writeData("angled-inline", token.current_token, token.tokenType()); // )
    }
    else
    {
        writeData("angled-inline", token.current_token, token.tokenType()); // .
        token.hasMoreTokens();
        writeData("angled-inline", token.current_token, token.tokenType()); // subroutineName
        token.hasMoreTokens();
        writeData("angled-inline", token.current_token, token.tokenType()); // (
        token.hasMoreTokens();
        if (std::string(token.current_token) != ")")
            CompileExpressionList();
        writeData("angled-inline", token.current_token, token.tokenType()); // )
    }

    token.hasMoreTokens();
    writeData("angled-inline", token.current_token, token.tokenType()); // ;

    indent(-1);
    writeData("close", "", "doStatement");
}

void analyzer::CompilationEngine::CompileReturn()
{
    writeData("angled", "", "returnStatement");
    indent(1);

    writeData("angled-inline", token.current_token, token.tokenType()); // return
    token.hasMoreTokens();

    if (std::string(token.current_token) != ";")
    {
        CompileExpression();
    }

    writeData("angled-inline", token.current_token, token.tokenType()); // ;

    indent(-1);
    writeData("close", "", "returnStatement");
}

void analyzer::CompilationEngine::CompileExpression()
{ // also fetchesj next token
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

void analyzer::CompilationEngine::Close(bool flag)
{

    if (flag == 0)
        std::cout << "Error with Syntax, aborting" << std::endl;

    filehandle.close();
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
