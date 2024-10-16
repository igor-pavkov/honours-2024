LIBSSH2_API int libssh2_session_supported_algs(LIBSSH2_SESSION* session,
                                               int method_type,
                                               const char*** algs)
{
    unsigned int i;
    unsigned int j;
    unsigned int ialg;
    const LIBSSH2_COMMON_METHOD **mlist;

    /* to prevent coredumps due to dereferencing of NULL */
    if (NULL == algs)
        return _libssh2_error(session, LIBSSH2_ERROR_BAD_USE,
                              "algs must not be NULL");

    switch (method_type) {
    case LIBSSH2_METHOD_KEX:
        mlist = (const LIBSSH2_COMMON_METHOD **) libssh2_kex_methods;
        break;

    case LIBSSH2_METHOD_HOSTKEY:
        mlist = (const LIBSSH2_COMMON_METHOD **) libssh2_hostkey_methods();
        break;

    case LIBSSH2_METHOD_CRYPT_CS:
    case LIBSSH2_METHOD_CRYPT_SC:
        mlist = (const LIBSSH2_COMMON_METHOD **) libssh2_crypt_methods();
        break;

    case LIBSSH2_METHOD_MAC_CS:
    case LIBSSH2_METHOD_MAC_SC:
        mlist = (const LIBSSH2_COMMON_METHOD **) _libssh2_mac_methods();
        break;

    case LIBSSH2_METHOD_COMP_CS:
    case LIBSSH2_METHOD_COMP_SC:
        mlist = (const LIBSSH2_COMMON_METHOD **) _libssh2_comp_methods(session);
        break;

    default:
        return _libssh2_error(session, LIBSSH2_ERROR_METHOD_NOT_SUPPORTED,
                              "Unknown method type");
    }  /* switch */

    /* weird situation */
    if (NULL==mlist)
        return _libssh2_error(session, LIBSSH2_ERROR_INVAL,
                              "No algorithm found");

    /*
      mlist is looped through twice. The first time to find the number od
      supported algorithms (needed to allocate the proper size of array) and
      the second time to actually copy the pointers.  Typically this function
      will not be called often (typically at the beginning of a session) and
      the number of algorithms (i.e. niumber of iterations in one loop) will
      not be high (typically it will not exceed 20) for quite a long time.

      So double looping really shouldn't be an issue and it is definitely a
      better solution than reallocation several times.
    */

    /* count the number of supported algorithms */
    for ( i=0, ialg=0; NULL!=mlist[i]; i++) {
        /* do not count fields with NULL name */
        if (mlist[i]->name)
            ialg++;
    }

    /* weird situation, no algorithm found */
    if (0==ialg)
        return _libssh2_error(session, LIBSSH2_ERROR_INVAL,
                              "No algorithm found");

    /* allocate buffer */
    *algs = (const char**) LIBSSH2_ALLOC(session, ialg*sizeof(const char*));
    if ( NULL==*algs ) {
        return _libssh2_error(session, LIBSSH2_ERROR_ALLOC,
                              "Memory allocation failed");
    }
    /* Past this point *algs must be deallocated in case of an error!! */

    /* copy non-NULL pointers only */
    for ( i=0, j=0; NULL!=mlist[i] && j<ialg; i++ ) {
        if ( NULL==mlist[i]->name ){
            /* maybe a weird situation but if it occurs, do not include NULL
               pointers */
            continue;
        }

        /* note that [] has higher priority than * (dereferencing) */
        (*algs)[j++] = mlist[i]->name;
    }

    /* correct number of pointers copied? (test the code above) */
    if ( j!=ialg ) {
        /* deallocate buffer */
        LIBSSH2_FREE(session, (void *)*algs);
        *algs = NULL;

        return _libssh2_error(session, LIBSSH2_ERROR_BAD_USE,
                              "Internal error");
    }

    return ialg;
}