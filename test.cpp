#include <iostream>
#include <string>
#include <map>
#include <Python.h>

static std::string& GetResultString( PyThreadState* threadState )
{
    static std::map< PyThreadState*, std::string > ResultStrings;
    if ( !ResultStrings.count( threadState ) )
    {
        ResultStrings[ threadState ] = "";
    }
    return ResultStrings[ threadState ];
}

static PyObject* redirector_init(PyObject *, PyObject *)
{
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject* redirector_write(PyObject *, PyObject *args)
{
    char* output;
    PyObject *selfi;

    if (!PyArg_ParseTuple(args,"Os",&selfi,&output))
    {
        return NULL;
    }

    std::string outputString( output );
    PyThreadState* currentThread = PyThreadState_Get( );
    std::string& resultString = GetResultString( currentThread );
    resultString = resultString + outputString;
    Py_INCREF(Py_None);
    return Py_None;
}

static PyMethodDef ModuleMethods[] = { {NULL,NULL,0,NULL} };
static PyMethodDef redirectorMethods[] =
{
    {"__init__", redirector_init, METH_VARARGS,
     "initialize the stdout/err redirector"},
    {"write", redirector_write, METH_VARARGS,
     "implement the write method to redirect stdout/err"},
    {NULL,NULL,0,NULL},
};

void initredirector( PyThreadState* threadState )
{
    PyMethodDef *def;

    /* create a new module and class */
    PyObject *module = Py_InitModule("redirector", ModuleMethods);
    PyObject *moduleDict = PyModule_GetDict(module);
    PyObject *classDict = PyDict_New();
    PyObject *className = PyString_FromString("redirector");
    PyObject *fooClass = PyClass_New(NULL, classDict, className);
    PyDict_SetItemString(moduleDict, "redirector", fooClass);
    Py_DECREF(classDict);
    Py_DECREF(className);
    Py_DECREF(fooClass);

    /* add methods to class */
    for (def = redirectorMethods; def->ml_name != NULL; def++) {
        PyObject *func = PyCFunction_New(def, NULL);
        PyObject *method = PyMethod_New(func, NULL, fooClass);
        PyDict_SetItemString(classDict, def->ml_name, method);
        Py_DECREF(func);
        Py_DECREF(method);
    }
}

class Interpreter
{
protected:
    static PyThreadState* MainThreadState;

    PyThreadState* m_threadState;
    PyObject* glb;
    PyObject* loc;

public:
    Interpreter( )
    {
        PyEval_AcquireLock( );
        m_threadState = Py_NewInterpreter( );

        PyObject *module = PyImport_ImportModule("__main__");
        loc = glb = PyModule_GetDict(module);
        initredirector( m_threadState );
        PyRun_SimpleString("import sys\n"
            "import redirector\n"
            "sys.path.insert(0, \".\")\n" // add current path
            "sys.stdout = redirector.redirector()\n"
            "sys.stderr = sys.stdout\n"
        );

        PyEval_ReleaseThread( m_threadState );
    }

    virtual ~Interpreter( )
    {
        PyEval_AcquireThread( m_threadState );

        Py_EndInterpreter( m_threadState );
        PyEval_ReleaseLock( );
    }

    void test( )
    {
        PyEval_AcquireThread( m_threadState );

        PyObject* py_result;
        PyObject* dum;
        std::string command = "print 'Hello world'\n";
        py_result = Py_CompileString(command.c_str(), "<stdin>", Py_single_input);
        if ( py_result == 0 )
        {
            std::cout << "Huh?\n";
            return;
        }
        dum = PyEval_EvalCode ((PyCodeObject *)py_result, glb, loc);
        Py_XDECREF (dum);
        Py_XDECREF (py_result);

        std::cout << GetResultString( m_threadState );

        PyEval_ReleaseThread( m_threadState );
    }

    std::string interpret( const std::string& command, int* errorCode )
    {
        PyEval_AcquireThread( m_threadState );

        PyObject* py_result;
        PyObject* dum;
        py_result = Py_CompileString(command.c_str(), "<stdin>", Py_single_input);
        if ( py_result == 0 )
        {
            std::cout << "Huh?\n";
            return "";
        }
        dum = PyEval_EvalCode ((PyCodeObject *)py_result, glb, loc);
        Py_XDECREF (dum);
        Py_XDECREF (py_result);

        std::string res = GetResultString( m_threadState );
        GetResultString( m_threadState ) = "";

        PyEval_ReleaseThread( m_threadState );
        return res;
    }

    static void Initialize( )
    {
        Py_Initialize( );
        PyEval_InitThreads( );
        MainThreadState = PyEval_SaveThread( );
    }

    static void Finalize( )
    {
        PyEval_RestoreThread( MainThreadState );
        Py_Finalize( );
    }
};

PyThreadState* Interpreter::MainThreadState = NULL;

int main( int argc, char *argv[] )
{
    std::string commands[] = {
        "from time import time,ctime\n",
        "print 'Today is',ctime(time())\n"
    };
    Interpreter::Initialize( );
    Interpreter* interpreter = new Interpreter;
    for ( int i = 0; i < 2; ++i )
    {
        int err;
        std::string res = interpreter->interpret( commands[i], &err );
        std::cout << res;
    }
    delete interpreter;

    Interpreter::Finalize( );
    return 0;
}
