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

ParseHelper::Indent::
Indent( )
{ }

ParseHelper::Indent::
Indent( const std::string& indent ):
    Token( indent )
{ }

ParseHelper::ParseState::
ParseState( ParseHelper& parent_ ): parent( parent_ )
{ }

ParseHelper::ParseState::
~ParseState( )
{ }

ParseHelper::BlockParseState::
BlockParseState( ParseHelper& parent ):
    ParseState( parent )
{ }

ParseHelper::BlockParseState::
BlockParseState( ParseHelper& parent, const std::string& indent_ ):
    ParseState( parent ),
    indent( indent_ )
{ }

bool ParseHelper::BlockParseState::
process(const std::string& str)
{
    bool ok = initializeIndent(str);
    if ( ! ok )
    {
        // finish processing
        return true;
    }

    Indent ind;
    bool isIndented = PeekIndent( str, &ind );
    if ( isIndented )
    {
#ifndef NDEBUG
        std::cout << "current line indent: ";
        print( ind );
#endif
        // check if indent matches
        if ( ind.Token != indent.Token )
        {
            // dedent until we match or empty the stack
            bool found = false;
            //while ( !found && indentStack.size( ) )
            while ( !found )
            {
                //currentIndent.Token = indentStack.back( ).Token;
                //indentStack.pop_back( );

                parent.stateStack.pop_back( );
                if ( !parent.stateStack.size( ) )
                    break;
                boost::shared_ptr<BlockParseState> parseState =
                    boost::dynamic_pointer_cast<BlockParseState>(
                        parent.stateStack.back( ));
                //parent.currentIndent.Token = parseState->indent.Token;
                found = ( ind.Token == parseState->indent.Token );
            }

            if ( ! found )
            {
#ifndef NDEBUG
                std::cout << "indent mismatch\n";
#endif
                parent.reset( );
                ParseMessage msg( 1, "IndentationError: unexpected indent");
                parent.broadcast( msg );
                return true;
            }
        }

        // process command

        // enter indented block state
        if ( str[str.size()-1] == ':' )
        {
            parent.commandBuffer.push_back( str );
            //parent.inBlock = (boost::dynamic_pointer_cast<BlockParseState>(
            //    parent.stateStack.back()));

            //expectingIndent = true;
            boost::shared_ptr<ParseState> parseState(
                new BlockParseState( parent ) );
            parent.stateStack.push_back( parseState );
            return true;
        }

        parent.commandBuffer.push_back( str );
        return true;
    }
    else
    {
#ifndef NDEBUG
        std::cout << "Leaving block\n";
#endif
        parent.flush( );
        parent.reset( );
        return false;
    }
}

bool ParseHelper::BlockParseState::
initializeIndent(const std::string& str)
{
    bool expectingIndent = (indent.Token == "");
    if ( !expectingIndent )
    {
        std::cout << "already initialized indent: ";
        print( indent );
        return true;
    }

    Indent ind;
    bool isIndented = parent.PeekIndent( str, &ind );
    if ( !isIndented )
    {
#ifndef NDEBUG
        std::cout << "Expected indented block\n";
#endif
        parent.reset( );
        ParseMessage msg( 1, "IndentationError: expected an indented block" );
        parent.broadcast( msg );
        return false;
    }
    indent = ind;
    //parent.currentIndent = ind;
    //parent.indentStack.push_back( parent.currentIndent );
    //parent.expectingIndent = false;
#ifndef NDEBUG
    std::cout << "initializing indent: ";
    print( ind );
#endif
    return true;
}

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
    //inBlock( false ),
    inContinuation( false )
    //expectingIndent( false )
{ }

void ParseHelper::process( const std::string& str )
{
#ifndef NDEBUG
    std::cout << "processing: (" << str << ")\n";
#endif

    boost::shared_ptr<BlockParseState> blockStatePtr;
    if (stateStack.size()
        && (blockStatePtr = boost::dynamic_pointer_cast<BlockParseState>(
            stateStack.back())))
    {
        if (blockStatePtr->process(str))
            return;
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
        //inBlock = true;
        //expectingIndent = true;

        boost::shared_ptr<ParseState> parseState(
            new BlockParseState( *this ) );
        stateStack.push_back( parseState );
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
    //inBlock = false;
    inContinuation = false;
    //expectingIndent = false;
    //indentStack.clear( );
    stateStack.clear( );
    //currentIndent.Token = "";
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
