static int kex_agree_crypt(LIBSSH2_SESSION * session,
                           libssh2_endpoint_data *endpoint,
                           unsigned char *crypt,
                           unsigned long crypt_len)
{
    const LIBSSH2_CRYPT_METHOD **cryptp = libssh2_crypt_methods();
    unsigned char *s;

    (void) session;

    if (endpoint->crypt_prefs) {
        s = (unsigned char *) endpoint->crypt_prefs;

        while (s && *s) {
            unsigned char *p = (unsigned char *) strchr((char *) s, ',');
            size_t method_len = (p ? (size_t)(p - s) : strlen((char *) s));

            if (kex_agree_instr(crypt, crypt_len, s, method_len)) {
                const LIBSSH2_CRYPT_METHOD *method =
                    (const LIBSSH2_CRYPT_METHOD *)
                    kex_get_method_by_name((char *) s, method_len,
                                           (const LIBSSH2_COMMON_METHOD **)
                                           cryptp);

                if (!method) {
                    /* Invalid method -- Should never be reached */
                    return -1;
                }

                endpoint->crypt = method;
                return 0;
            }

            s = p ? p + 1 : NULL;
        }
        return -1;
    }

    while (*cryptp && (*cryptp)->name) {
        s = kex_agree_instr(crypt, crypt_len,
                            (unsigned char *) (*cryptp)->name,
                            strlen((*cryptp)->name));
        if (s) {
            endpoint->crypt = *cryptp;
            return 0;
        }
        cryptp++;
    }

    return -1;
}