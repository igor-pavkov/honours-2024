static int kex_agree_hostkey(LIBSSH2_SESSION * session,
                             unsigned long kex_flags,
                             unsigned char *hostkey, unsigned long hostkey_len)
{
    const LIBSSH2_HOSTKEY_METHOD **hostkeyp = libssh2_hostkey_methods();
    unsigned char *s;

    if (session->hostkey_prefs) {
        s = (unsigned char *) session->hostkey_prefs;

        while (s && *s) {
            unsigned char *p = (unsigned char *) strchr((char *) s, ',');
            size_t method_len = (p ? (size_t)(p - s) : strlen((char *) s));
            if (kex_agree_instr(hostkey, hostkey_len, s, method_len)) {
                const LIBSSH2_HOSTKEY_METHOD *method =
                    (const LIBSSH2_HOSTKEY_METHOD *)
                    kex_get_method_by_name((char *) s, method_len,
                                           (const LIBSSH2_COMMON_METHOD **)
                                           hostkeyp);

                if (!method) {
                    /* Invalid method -- Should never be reached */
                    return -1;
                }

                /* So far so good, but does it suit our purposes? (Encrypting
                   vs Signing) */
                if (((kex_flags & LIBSSH2_KEX_METHOD_FLAG_REQ_ENC_HOSTKEY) ==
                     0) || (method->encrypt)) {
                    /* Either this hostkey can do encryption or this kex just
                       doesn't require it */
                    if (((kex_flags & LIBSSH2_KEX_METHOD_FLAG_REQ_SIGN_HOSTKEY)
                         == 0) || (method->sig_verify)) {
                        /* Either this hostkey can do signing or this kex just
                           doesn't require it */
                        session->hostkey = method;
                        return 0;
                    }
                }
            }

            s = p ? p + 1 : NULL;
        }
        return -1;
    }

    while (hostkeyp && (*hostkeyp) && (*hostkeyp)->name) {
        s = kex_agree_instr(hostkey, hostkey_len,
                            (unsigned char *) (*hostkeyp)->name,
                            strlen((*hostkeyp)->name));
        if (s) {
            /* So far so good, but does it suit our purposes? (Encrypting vs
               Signing) */
            if (((kex_flags & LIBSSH2_KEX_METHOD_FLAG_REQ_ENC_HOSTKEY) == 0) ||
                ((*hostkeyp)->encrypt)) {
                /* Either this hostkey can do encryption or this kex just
                   doesn't require it */
                if (((kex_flags & LIBSSH2_KEX_METHOD_FLAG_REQ_SIGN_HOSTKEY) ==
                     0) || ((*hostkeyp)->sig_verify)) {
                    /* Either this hostkey can do signing or this kex just
                       doesn't require it */
                    session->hostkey = *hostkeyp;
                    return 0;
                }
            }
        }
        hostkeyp++;
    }

    return -1;
}