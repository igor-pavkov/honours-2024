static int kex_agree_mac(LIBSSH2_SESSION * session,
                         libssh2_endpoint_data * endpoint, unsigned char *mac,
                         unsigned long mac_len)
{
    const LIBSSH2_MAC_METHOD **macp = _libssh2_mac_methods();
    unsigned char *s;
    (void) session;

    if (endpoint->mac_prefs) {
        s = (unsigned char *) endpoint->mac_prefs;

        while (s && *s) {
            unsigned char *p = (unsigned char *) strchr((char *) s, ',');
            size_t method_len = (p ? (size_t)(p - s) : strlen((char *) s));

            if (kex_agree_instr(mac, mac_len, s, method_len)) {
                const LIBSSH2_MAC_METHOD *method = (const LIBSSH2_MAC_METHOD *)
                    kex_get_method_by_name((char *) s, method_len,
                                           (const LIBSSH2_COMMON_METHOD **)
                                           macp);

                if (!method) {
                    /* Invalid method -- Should never be reached */
                    return -1;
                }

                endpoint->mac = method;
                return 0;
            }

            s = p ? p + 1 : NULL;
        }
        return -1;
    }

    while (*macp && (*macp)->name) {
        s = kex_agree_instr(mac, mac_len, (unsigned char *) (*macp)->name,
                            strlen((*macp)->name));
        if (s) {
            endpoint->mac = *macp;
            return 0;
        }
        macp++;
    }

    return -1;
}