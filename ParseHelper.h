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

#ifndef PARSE_HELPER_H
#define PARSE_HELPER_H
#include <string>
#include <vector>
#include "ParseMessage.h"

class ParseListener;

/**
Helps chunk lines of Python code into compilable statements.
*/
class ParseHelper
{
public:
    struct Indent
    {
        std::string Token;
    };

protected:
    bool inBlock;
    bool expectingIndent;
    std::vector< Indent > indentStack;
    std::vector< ParseListener* > listeners;
    Indent currentIndent;
    std::vector< std::string > commandBuffer;

public:
    static bool PeekIndent( const std::string& str, Indent* indent );

public:
    ParseHelper( );

public:
    void process( const std::string& str );

    /**
    Generate a parse event from the current command buffer.
    */
    void flush( );

    /**
    Reset the state of the helper.
    */
    void reset( );

    void subscribe( ParseListener* listener );
    void unsubscribeAll( );
    void broadcast( const ParseMessage& msg );

}; // class ParseHelper

inline bool operator== ( const ParseHelper::Indent& a, const ParseHelper::Indent& b )
{
    return a.Token == b.Token;
}

inline bool operator!= ( const ParseHelper::Indent& a, const ParseHelper::Indent& b )
{
    return a.Token != b.Token;
}

#ifndef NDEBUG
void print(const ParseHelper::Indent& indent);
#endif

#endif // PARSE_HELPER_H
