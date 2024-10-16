static int kex_agree_methods(LIBSSH2_SESSION * session, unsigned char *data,
                             unsigned data_len)
{
    unsigned char *kex, *hostkey, *crypt_cs, *crypt_sc, *comp_cs, *comp_sc,
        *mac_cs, *mac_sc;
    size_t kex_len, hostkey_len, crypt_cs_len, crypt_sc_len, comp_cs_len;
    size_t comp_sc_len, mac_cs_len, mac_sc_len;
    unsigned char *s = data;

    /* Skip packet_type, we know it already */
    s++;

    /* Skip cookie, don't worry, it's preserved in the kexinit field */
    s += 16;

    /* Locate each string */
    if(kex_string_pair(&s, data, data_len, &kex_len, &kex))
        return -1;
    if(kex_string_pair(&s, data, data_len, &hostkey_len, &hostkey))
        return -1;
    if(kex_string_pair(&s, data, data_len, &crypt_cs_len, &crypt_cs))
        return -1;
    if(kex_string_pair(&s, data, data_len, &crypt_sc_len, &crypt_sc))
        return -1;
    if(kex_string_pair(&s, data, data_len, &mac_cs_len, &mac_cs))
        return -1;
    if(kex_string_pair(&s, data, data_len, &mac_sc_len, &mac_sc))
        return -1;
    if(kex_string_pair(&s, data, data_len, &comp_cs_len, &comp_cs))
        return -1;
    if(kex_string_pair(&s, data, data_len, &comp_sc_len, &comp_sc))
        return -1;

    /* If the server sent an optimistic packet, assume that it guessed wrong.
     * If the guess is determined to be right (by kex_agree_kex_hostkey)
     * This flag will be reset to zero so that it's not ignored */
    session->burn_optimistic_kexinit = *(s++);
    /* Next uint32 in packet is all zeros (reserved) */

    if (data_len < (unsigned) (s - data))
        return -1;              /* short packet */

    if (kex_agree_kex_hostkey(session, kex, kex_len, hostkey, hostkey_len)) {
        return -1;
    }

    if (kex_agree_crypt(session, &session->local, crypt_cs, crypt_cs_len)
        || kex_agree_crypt(session, &session->remote, crypt_sc, crypt_sc_len)) {
        return -1;
    }

    if (kex_agree_mac(session, &session->local, mac_cs, mac_cs_len) ||
        kex_agree_mac(session, &session->remote, mac_sc, mac_sc_len)) {
        return -1;
    }

    if (kex_agree_comp(session, &session->local, comp_cs, comp_cs_len) ||
        kex_agree_comp(session, &session->remote, comp_sc, comp_sc_len)) {
        return -1;
    }

#if 0
    if (libssh2_kex_agree_lang(session, &session->local, lang_cs, lang_cs_len)
        || libssh2_kex_agree_lang(session, &session->remote, lang_sc,
                                  lang_sc_len)) {
        return -1;
    }
#endif

    _libssh2_debug(session, LIBSSH2_TRACE_KEX, "Agreed on KEX method: %s",
                   session->kex->name);
    _libssh2_debug(session, LIBSSH2_TRACE_KEX, "Agreed on HOSTKEY method: %s",
                   session->hostkey->name);
    _libssh2_debug(session, LIBSSH2_TRACE_KEX, "Agreed on CRYPT_CS method: %s",
                   session->local.crypt->name);
    _libssh2_debug(session, LIBSSH2_TRACE_KEX, "Agreed on CRYPT_SC method: %s",
                   session->remote.crypt->name);
    _libssh2_debug(session, LIBSSH2_TRACE_KEX, "Agreed on MAC_CS method: %s",
                   session->local.mac->name);
    _libssh2_debug(session, LIBSSH2_TRACE_KEX, "Agreed on MAC_SC method: %s",
                   session->remote.mac->name);
    _libssh2_debug(session, LIBSSH2_TRACE_KEX, "Agreed on COMP_CS method: %s",
                   session->local.comp->name);
    _libssh2_debug(session, LIBSSH2_TRACE_KEX, "Agreed on COMP_SC method: %s",
                   session->remote.comp->name);

    return 0;
}