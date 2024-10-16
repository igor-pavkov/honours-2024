static int do_x509_check(X509 *x, const char *chk, size_t chklen,
                         unsigned int flags, int check_type, char **peername)
{
    GENERAL_NAMES *gens = NULL;
    X509_NAME *name = NULL;
    int i;
    int cnid = NID_undef;
    int alt_type;
    int san_present = 0;
    int rv = 0;
    equal_fn equal;

    /* See below, this flag is internal-only */
    flags &= ~_X509_CHECK_FLAG_DOT_SUBDOMAINS;
    if (check_type == GEN_EMAIL) {
        cnid = NID_pkcs9_emailAddress;
        alt_type = V_ASN1_IA5STRING;
        equal = equal_email;
    } else if (check_type == GEN_DNS) {
        cnid = NID_commonName;
        /* Implicit client-side DNS sub-domain pattern */
        if (chklen > 1 && chk[0] == '.')
            flags |= _X509_CHECK_FLAG_DOT_SUBDOMAINS;
        alt_type = V_ASN1_IA5STRING;
        if (flags & X509_CHECK_FLAG_NO_WILDCARDS)
            equal = equal_nocase;
        else
            equal = equal_wildcard;
    } else {
        alt_type = V_ASN1_OCTET_STRING;
        equal = equal_case;
    }

    if (chklen == 0)
        chklen = strlen(chk);

    gens = X509_get_ext_d2i(x, NID_subject_alt_name, NULL, NULL);
    if (gens) {
        for (i = 0; i < sk_GENERAL_NAME_num(gens); i++) {
            GENERAL_NAME *gen;
            ASN1_STRING *cstr;
            gen = sk_GENERAL_NAME_value(gens, i);
            if (gen->type != check_type)
                continue;
            san_present = 1;
            if (check_type == GEN_EMAIL)
                cstr = gen->d.rfc822Name;
            else if (check_type == GEN_DNS)
                cstr = gen->d.dNSName;
            else
                cstr = gen->d.iPAddress;
            /* Positive on success, negative on error! */
            if ((rv = do_check_string(cstr, alt_type, equal, flags,
                                      chk, chklen, peername)) != 0)
                break;
        }
        GENERAL_NAMES_free(gens);
        if (rv != 0)
            return rv;
        if (san_present && !(flags & X509_CHECK_FLAG_ALWAYS_CHECK_SUBJECT))
            return 0;
    }

    /* We're done if CN-ID is not pertinent */
    if (cnid == NID_undef || (flags & X509_CHECK_FLAG_NEVER_CHECK_SUBJECT))
        return 0;

    i = -1;
    name = X509_get_subject_name(x);
    while ((i = X509_NAME_get_index_by_NID(name, cnid, i)) >= 0) {
        const X509_NAME_ENTRY *ne = X509_NAME_get_entry(name, i);
        const ASN1_STRING *str = X509_NAME_ENTRY_get_data(ne);

        /* Positive on success, negative on error! */
        if ((rv = do_check_string(str, -1, equal, flags,
                                  chk, chklen, peername)) != 0)
            return rv;
    }
    return 0;
}