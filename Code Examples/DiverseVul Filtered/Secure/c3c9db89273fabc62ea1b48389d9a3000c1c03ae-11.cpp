void _Py_ReleaseInternedStrings(void)
{
    PyObject *keys;
    PyStringObject *s;
    Py_ssize_t i, n;
    Py_ssize_t immortal_size = 0, mortal_size = 0;

    if (interned == NULL || !PyDict_Check(interned))
        return;
    keys = PyDict_Keys(interned);
    if (keys == NULL || !PyList_Check(keys)) {
        PyErr_Clear();
        return;
    }

    /* Since _Py_ReleaseInternedStrings() is intended to help a leak
       detector, interned strings are not forcibly deallocated; rather, we
       give them their stolen references back, and then clear and DECREF
       the interned dict. */

    n = PyList_GET_SIZE(keys);
    fprintf(stderr, "releasing %" PY_FORMAT_SIZE_T "d interned strings\n",
        n);
    for (i = 0; i < n; i++) {
        s = (PyStringObject *) PyList_GET_ITEM(keys, i);
        switch (s->ob_sstate) {
        case SSTATE_NOT_INTERNED:
            /* XXX Shouldn't happen */
            break;
        case SSTATE_INTERNED_IMMORTAL:
            Py_REFCNT(s) += 1;
            immortal_size += Py_SIZE(s);
            break;
        case SSTATE_INTERNED_MORTAL:
            Py_REFCNT(s) += 2;
            mortal_size += Py_SIZE(s);
            break;
        default:
            Py_FatalError("Inconsistent interned string state.");
        }
        s->ob_sstate = SSTATE_NOT_INTERNED;
    }
    fprintf(stderr, "total size of all interned strings: "
                    "%" PY_FORMAT_SIZE_T "d/%" PY_FORMAT_SIZE_T "d "
                    "mortal/immortal\n", mortal_size, immortal_size);
    Py_DECREF(keys);
    PyDict_Clear(interned);
    Py_CLEAR(interned);
}