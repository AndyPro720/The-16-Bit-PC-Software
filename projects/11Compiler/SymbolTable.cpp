/* Symbol table works in tandem with the tokenizer and compilation engine to keep track of variables, their types,
    and scope.
    Run via JackCompiler.exe

Intended to be run for Jack the object oriented language, created in tandem with Hack, the custom 16-Bit-PC
*/

#include "JackCompiler.h"

analyzer::SymbolTable::SymbolTable() : staticCount(0), fieldCount(0), argCount(0), varCount(0)
{
}

void analyzer::SymbolTable::StartSubroutine()
{ // resets the subroutine symbols for a new subroutine
    subroutineSymbols.clear();
    argCount = 0;
    varCount = 0;
}

void analyzer::SymbolTable::Define(const std::string &name, const std::string &type, symbolKind kind)
{ // defines a new symbol in the current scope (class or subroutine)

    bool inserted;
    switch (kind)
    {

    case symbolKind::STATIC:
        inserted = classSymbols.emplace(name, symbolInfo{type, kind, staticCount++}).second; // using emplace to avoid unnecessary copies
        break;

    case symbolKind::FIELD:
        inserted = classSymbols.emplace(name, symbolInfo{type, kind, fieldCount++}).second;
        break;

    case symbolKind::ARG:
        inserted = subroutineSymbols.emplace(name, symbolInfo{type, kind, argCount++}).second;
        break;

    case symbolKind::VAR:
        inserted = subroutineSymbols.emplace(name, symbolInfo{type, kind, varCount++}).second;
        break;
    }

    if (!inserted) // if duplicate declaration and ignore count++
    {
        throw std::runtime_error("Duplicate declaration of symbol: " + name);
    }
}

analyzer::symbolKind analyzer::SymbolTable::KindOf(const std::string &name)
{ // checks if symbol exists in subroutine or class scope and returns its kind
    auto it = subroutineSymbols.find(name);

    if (it == subroutineSymbols.end())
    {
        it = classSymbols.find(name);
        if (it == classSymbols.end())
        {
            return analyzer::symbolKind::NONE;
        }
    }

    return it->second.kind;
}

std::string analyzer::SymbolTable::TypeOf(const std::string &name)
{ // checks if symbol exists in subroutine or class scope and returns its type
    auto it = subroutineSymbols.find(name);

    if (it == subroutineSymbols.end())
    {
        it = classSymbols.find(name);
        if (it == classSymbols.end())
        {
            throw std::runtime_error("Symbol not found yet TypeOf fetched " + name);
        }
    }

    return it->second.type;
}

int analyzer::SymbolTable::IndexOf(const std::string &name)
{ // checks if symbol exists in subroutine or class scope and returns its index
    auto it = subroutineSymbols.find(name);

    if (it == subroutineSymbols.end())
    {
        it = classSymbols.find(name);
        if (it == classSymbols.end())
        {
            throw std::runtime_error("Symbol not found yet IndexOf fetched " + name);
        }
    }

    return it->second.index;
}

int analyzer::SymbolTable::VarCount(symbolKind kind)
{ // returns the count of variables of a specific kind in the current scope
    switch (kind)
    {
    case symbolKind::STATIC:
        return staticCount;
    case symbolKind::FIELD:
        return fieldCount;
    case symbolKind::ARG:
        return argCount;
    case symbolKind::VAR:
        return varCount;
    default:
        return 0; // NONE or any other kind
    }
}