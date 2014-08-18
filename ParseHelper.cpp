/**
Copyright (c) 2014 Alex Tsui

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/
#include "ParseHelper.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <cstdlib>
#include "ParseListener.h"

#ifndef NDEBUG
void print(const ParseHelper::Indent& indent)
{
    std::string str = indent.Token;
    for (int i = 0; i < str.size(); ++i)
    {
        switch (str.at(i))
        {
            case ' ':
                str[i] = 's';
                break;
            case '\t':
                str[i] = 't';
                break;
        }
    }
    std::cout << str << "\n";
}
#endif

bool ParseHelper::PeekIndent( const std::string& str, Indent* indent )
{
    if ( !str.size() || ! isspace(str[0]) )
        return false;

    int nonwhitespaceIndex = -1;
    for (int i = 0; i < str.size(); ++i)
    {
        if (!isspace(str[i]))
        {
            nonwhitespaceIndex = i;
            break;
        }
    }
    if (nonwhitespaceIndex == -1)
    {
        return false;
    }
    std::string indentToken = str.substr(0, nonwhitespaceIndex);
    indent->Token = indentToken;
    return true;
}

ParseHelper::ParseHelper( ):
    inBlock( false ),
    inContinuation( false ),
    expectingIndent( false )
{ }

void ParseHelper::process( const std::string& str )
{
#ifndef NDEBUG
    std::cout << "processing: (" << str << ")\n";
#endif
    if ( expectingIndent )
    {
        Indent ind;
        bool isIndented = PeekIndent( str, &ind );
        if ( !isIndented )
        {
#ifndef NDEBUG
            std::cout << "Expected indented block\n";
#endif
            reset( );
            ParseMessage msg( 1, "IndentationError: expected an indented block" );
            broadcast( msg );
            return;
        }
        currentIndent = ind;
        indentStack.push_back( currentIndent );
        expectingIndent = false;
    }

    if ( inBlock )
    {
        Indent ind;
        bool isIndented = PeekIndent( str, &ind );
        if ( isIndented )
        {
#ifndef NDEBUG
            print( ind );
#endif
            // check if indent matches
            if ( ind.Token != currentIndent.Token )
            {
                // dedent until we match or empty the stack
                bool found = false;
                while ( !found && indentStack.size( ) )
                {
                    currentIndent.Token = indentStack.back( ).Token;
                    indentStack.pop_back( );
                    found = ( ind.Token == currentIndent.Token );
                }

                if ( ! found )
                {
#ifndef NDEBUG
                    std::cout << "indent mismatch\n";
#endif
                    reset( );
                    ParseMessage msg( 1, "IndentationError: unexpected indent");
                    broadcast( msg );
                    return;
                }
            }

            // process command

            // enter indented block state
            if ( str[str.size()-1] == ':' )
            {
                commandBuffer.push_back( str );
                inBlock = true;
                expectingIndent = true;
                return;
            }

            commandBuffer.push_back( str );
            return;
        }
        else
        {
#ifndef NDEBUG
            std::cout << "Leaving block\n";
#endif
            flush( );
            reset( );
        }
    }

    // standard state
    if ( !str.size() )
        return;

    { // check for unexpected indent
        Indent ind;
        bool isIndented = PeekIndent( str, &ind );
        if ( isIndented &&
            ! inContinuation )
        {
            reset( );
            ParseMessage msg( 1, "IndentationError: unexpected indent");
            broadcast( msg );
            return;
        }
    }

    // enter indented block state
    if ( str[str.size()-1] == ':' )
    {
        commandBuffer.push_back( str );
        inBlock = true;
        expectingIndent = true;
        return;
    }

    if ( str[str.size()-1] == '\\' )
    {
        commandBuffer.push_back( str );
        inContinuation = true;
        return;
    }

    // handle single-line statement
    commandBuffer.push_back( str );
    flush( );
}

bool ParseHelper::buffered( ) const
{
    return commandBuffer.size( );
}

void ParseHelper::flush( )
{
    std::stringstream ss;
    for (int i = 0; i < commandBuffer.size(); ++i )
    {
        ss << commandBuffer[i] << "\n";
    }
    commandBuffer.clear();

    broadcast( ss.str() );
    // TODO: feed string to interpreter
}

void ParseHelper::reset( )
{
    inBlock = false;
    inContinuation = false;
    expectingIndent = false;
    indentStack.clear( );
    currentIndent.Token = "";
    commandBuffer.clear( );
}

void ParseHelper::subscribe( ParseListener* listener )
{
    listeners.push_back( listener );
}

void ParseHelper::unsubscribeAll( )
{
    listeners.clear( );
}

void ParseHelper::broadcast( const ParseMessage& msg )
{
    // broadcast signal
    for (int i = 0; i < listeners.size(); ++i)
    {
        if (listeners[i])
        {
            listeners[i]->parseEvent(msg);
        }
    }
}
