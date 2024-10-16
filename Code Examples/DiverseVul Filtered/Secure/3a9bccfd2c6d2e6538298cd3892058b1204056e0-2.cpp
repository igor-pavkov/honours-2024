    void init_psutil_posix(void)
#endif  /* PY_MAJOR_VERSION */
{
#if PY_MAJOR_VERSION >= 3
    PyObject *mod = PyModule_Create(&moduledef);
#else
    PyObject *mod = Py_InitModule("_psutil_posix", mod_methods);
#endif
    if (mod == NULL)
        INITERR;

#if defined(PSUTIL_BSD) || defined(PSUTIL_OSX) || defined(PSUTIL_SUNOS) || defined(PSUTIL_AIX)
    if (PyModule_AddIntConstant(mod, "AF_LINK", AF_LINK)) INITERR;
#endif

    if (mod == NULL)
        INITERR;
#if PY_MAJOR_VERSION >= 3
    return mod;
#endif
}