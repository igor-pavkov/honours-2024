static STACK_OF(OPENSSL_STRING) *get_email(X509_NAME *name,
                                           GENERAL_NAMES *gens)
{
    STACK_OF(OPENSSL_STRING) *ret = NULL;
    X509_NAME_ENTRY *ne;
    const ASN1_IA5STRING *email;
    GENERAL_NAME *gen;
    int i = -1;

    /* Now add any email address(es) to STACK */
    /* First supplied X509_NAME */
    while ((i = X509_NAME_get_index_by_NID(name,
                                           NID_pkcs9_emailAddress, i)) >= 0) {
        ne = X509_NAME_get_entry(name, i);
        email = X509_NAME_ENTRY_get_data(ne);
        if (!append_ia5(&ret, email))
            return NULL;
    }
    for (i = 0; i < sk_GENERAL_NAME_num(gens); i++) {
        gen = sk_GENERAL_NAME_value(gens, i);
        if (gen->type != GEN_EMAIL)
            continue;
        if (!append_ia5(&ret, gen->d.ia5))
            return NULL;
    }
    return ret;
}