#include <Python.h>
#include "../libraries/ucomm/src/ucomm.h"

/**
 * \code{.py}
 * init()
 * \endcode
 */
static PyObject *
PyUcomm_init(PyObject *module, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = { NULL };

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "", kwlist))
        return NULL;

    ucomm_init();

    Py_RETURN_NONE;
}

/**
 * \code{.py}
 * readFrame() -> bytes
 * \endcode
 */
static PyObject *
PyUcomm_readFrame(PyObject *module, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = { NULL };
    uint8_t buf[255];

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "", kwlist))
        return NULL;

    uint8_t nbytes;
    Py_BEGIN_ALLOW_THREADS
    nbytes = uframe_read(buf, sizeof(buf));
    Py_END_ALLOW_THREADS

    return PyBytes_FromStringAndSize((const char *)buf, nbytes);
}

/**
 * \code{.py}
 * writeFrame(datagram: bytes)
 * \endcode
 */
static PyObject *
PyUcomm_writeFrame(PyObject *module, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = { "", NULL };
    Py_buffer pybuf = {};

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "y*", kwlist, &pybuf))
        return NULL;

    if (pybuf.len > 255) {
        PyBuffer_Release(&pybuf);
        return PyErr_Format(PyExc_BufferError,
            "buffer too large, can only be up to 255 bytes. got: %zd",
            pybuf.len);
    }

    Py_BEGIN_ALLOW_THREADS
    uframe_write(pybuf.buf, pybuf.len);
    Py_END_ALLOW_THREADS

    PyBuffer_Release(&pybuf);
    Py_RETURN_NONE;
}

static PyMethodDef PyUcomm_Methods[] = {
    {"init",
     (PyCFunction)PyUcomm_init,
     METH_VARARGS | METH_KEYWORDS,
     "Initialize UART communications. Must be called first.\n"
    },

    {"read_frame",
     (PyCFunction)PyUcomm_readFrame,
     METH_VARARGS | METH_KEYWORDS,
     "Reads a single frame. Will block until a frame can be read.\n"
    },

    {"write_frame",
    (PyCFunction)PyUcomm_writeFrame,
    METH_VARARGS | METH_KEYWORDS,
    "Write a single frame.\n"
    },

    {0}
};

static PyModuleDef PyUcomm_Module = {
    PyModuleDef_HEAD_INIT,
    /* m_name */ "ucomm",
    /* m_doc */ "UART communication library",
    /* m_size */ -1,
    /* m_methods */ PyUcomm_Methods,
    /* m_reload */ NULL,
    /* m_traverse */ NULL,
    /* m_clear */ NULL,
    /* m_free */ NULL
};

PyMODINIT_FUNC
PyInit__ucomm(void)
{
    PyObject *m = PyModule_Create(&PyUcomm_Module);

    if (!m)
        return NULL;

    return m;
}
