static int x509v3_add_len_value(const char *name, const char *value,
                                size_t vallen, STACK_OF(CONF_VALUE) **extlist)
{
    CONF_VALUE *vtmp = NULL;
    char *tname = NULL, *tvalue = NULL;
    int sk_allocated = (*extlist == NULL);

    if (name != NULL && (tname = OPENSSL_strdup(name)) == NULL)
        goto err;
    if (value != NULL && vallen > 0) {
        /*
         * We tolerate a single trailing NUL character, but otherwise no
         * embedded NULs
         */
        if (memchr(value, 0, vallen - 1) != NULL)
            goto err;
        tvalue = OPENSSL_strndup(value, vallen);
        if (tvalue == NULL)
            goto err;
    }
    if ((vtmp = OPENSSL_malloc(sizeof(*vtmp))) == NULL)
        goto err;
    if (sk_allocated && (*extlist = sk_CONF_VALUE_new_null()) == NULL)
        goto err;
    vtmp->section = NULL;
    vtmp->name = tname;
    vtmp->value = tvalue;
    if (!sk_CONF_VALUE_push(*extlist, vtmp))
        goto err;
    return 1;
 err:
    X509V3err(X509V3_F_X509V3_ADD_VALUE, ERR_R_MALLOC_FAILURE);
    if (sk_allocated) {
        sk_CONF_VALUE_free(*extlist);
        *extlist = NULL;
    }
    OPENSSL_free(vtmp);
    OPENSSL_free(tname);
    OPENSSL_free(tvalue);
    return 0;
}