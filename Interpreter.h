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
