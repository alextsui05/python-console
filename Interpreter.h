/**
python-console
Copyright (C) 2014  Alex Tsui

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef INTERPRETER_H
#define INTERPRETER_H
#include <string>
#include <Python.h>
#if defined(__APPLE__) && defined(__MACH__)
/*
 * The following undefs for C standard library macros prevent
 * build errors of the following type on mac ox 10.7.4 and XCode 4.3.3
 *
/usr/include/c++/4.2.1/bits/localefwd.h:57:21: error: too many arguments provided to function-like macro invocation
    isspace(_CharT, const locale&);
                    ^
/usr/include/c++/4.2.1/bits/localefwd.h:56:5: error: 'inline' can only appear on functions
    inline bool
    ^
/usr/include/c++/4.2.1/bits/localefwd.h:57:5: error: variable 'isspace' declared as a template
    isspace(_CharT, const locale&);
    ^
*/
#undef isspace
#undef isupper
#undef islower
#undef isalpha
#undef isalnum
#undef toupper
#undef tolower
#endif

/**
Wraps a Python interpreter, which you can pass commands as strings to interpret
and get strings of output/error in return.
*/
class Interpreter
{
protected:
    static PyThreadState* MainThreadState;

    PyThreadState* m_threadState;
    PyObject* glb;
    PyObject* loc;

public:
    /**
    Instantiate a Python interpreter.
    */
    Interpreter( );
    virtual ~Interpreter( );

    void test( );
    std::string interpret( const std::string& command, int* errorCode );

    /**
    Call this before constructing and using Interpreter.
    */
    static void Initialize( );

    /**
    Call this when done using Interpreter.
    */
    static void Finalize( );

protected:
    static void SetupRedirector( PyThreadState* threadState );
    static PyObject* RedirectorInit(PyObject *, PyObject *);
    static PyObject* RedirectorWrite(PyObject *, PyObject *args);
    static std::string& GetResultString( PyThreadState* threadState );
    static PyMethodDef ModuleMethods[];
    static PyMethodDef RedirectorMethods[];
};
#endif // INTERPRETER_H
