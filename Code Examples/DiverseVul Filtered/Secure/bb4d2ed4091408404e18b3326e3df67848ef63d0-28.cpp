int X509V3_add_value_int(const char *name, const ASN1_INTEGER *aint,
                         STACK_OF(CONF_VALUE) **extlist)
{
    char *strtmp;
    int ret;

    if (!aint)
        return 1;
    if ((strtmp = i2s_ASN1_INTEGER(NULL, aint)) == NULL)
        return 0;
    ret = X509V3_add_value(name, strtmp, extlist);
    OPENSSL_free(strtmp);
    return ret;
}