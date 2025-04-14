#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include "picohttpparser.h"

static PyObject* parse_http_request(PyObject* self, PyObject* args) {
    const char* request;
    Py_ssize_t request_len;

    // Parse Python arguments
    if (!PyArg_ParseTuple(args, "s#", &request, &request_len)) {
        return NULL;
    }

    // Buffer for headers
    struct phr_header headers[100];
    size_t num_headers = sizeof(headers) / sizeof(headers[0]);
    const char *method, *path;
    size_t method_len, path_len;
    int minor_version;
    int pret;

    // Parse HTTP request
    pret = phr_parse_request(request, request_len, &method, &method_len, &path, &path_len,
                             &minor_version, headers, &num_headers, 0);

    if (pret < 0) {
        return Py_BuildValue("s", "Error parsing request");
    }

    // Return parsed data as a Python dictionary
    PyObject* result = PyDict_New();
    PyDict_SetItemString(result, "method", Py_BuildValue("s#", method, method_len));
    PyDict_SetItemString(result, "path", Py_BuildValue("s#", path, path_len));
    PyDict_SetItemString(result, "minor_version", Py_BuildValue("i", minor_version));

    PyObject* headers_list = PyList_New(num_headers);
    for (size_t i = 0; i < num_headers; i++) {
        PyObject* header = Py_BuildValue("(s#s#)",
                                         headers[i].name, headers[i].name_len,
                                         headers[i].value, headers[i].value_len);
        PyList_SetItem(headers_list, i, header);
    }
    PyDict_SetItemString(result, "headers", headers_list);

    return result;
}

// Method definitions
static PyMethodDef PicoHTTPParserMethods[] = {
    {"parse_http_request", parse_http_request, METH_VARARGS, "Parse an HTTP request"},
    {NULL, NULL, 0, NULL}
};

// Module definition
static struct PyModuleDef picohttpparsermodule = {
    PyModuleDef_HEAD_INIT,
    "picohttpparser",
    NULL,
    -1,
    PicoHTTPParserMethods
};

// Module initialization
PyMODINIT_FUNC PyInit_picohttpparser(void) {
    return PyModule_Create(&picohttpparsermodule);
}
