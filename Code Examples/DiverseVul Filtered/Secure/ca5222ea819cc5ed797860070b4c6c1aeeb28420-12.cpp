static int kex_agree_comp(LIBSSH2_SESSION *session,
                          libssh2_endpoint_data *endpoint, unsigned char *comp,
                          unsigned long comp_len)
{
    const LIBSSH2_COMP_METHOD **compp = _libssh2_comp_methods(session);
    unsigned char *s;
    (void) session;

    if (endpoint->comp_prefs) {
        s = (unsigned char *) endpoint->comp_prefs;

        while (s && *s) {
            unsigned char *p = (unsigned char *) strchr((char *) s, ',');
            size_t method_len = (p ? (size_t)(p - s) : strlen((char *) s));

            if (kex_agree_instr(comp, comp_len, s, method_len)) {
                const LIBSSH2_COMP_METHOD *method =
                    (const LIBSSH2_COMP_METHOD *)
                    kex_get_method_by_name((char *) s, method_len,
                                           (const LIBSSH2_COMMON_METHOD **)
                                           compp);

                if (!method) {
                    /* Invalid method -- Should never be reached */
                    return -1;
                }

                endpoint->comp = method;
                return 0;
            }

            s = p ? p + 1 : NULL;
        }
        return -1;
    }

    while (*compp && (*compp)->name) {
        s = kex_agree_instr(comp, comp_len, (unsigned char *) (*compp)->name,
                            strlen((*compp)->name));
        if (s) {
            endpoint->comp = *compp;
            return 0;
        }
        compp++;
    }

    return -1;
}