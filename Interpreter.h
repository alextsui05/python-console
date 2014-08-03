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
