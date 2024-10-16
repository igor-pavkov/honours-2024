STACK_OF(OPENSSL_STRING) *X509_REQ_get1_email(X509_REQ *x)
{
    GENERAL_NAMES *gens;
    STACK_OF(X509_EXTENSION) *exts;
    STACK_OF(OPENSSL_STRING) *ret;

    exts = X509_REQ_get_extensions(x);
    gens = X509V3_get_d2i(exts, NID_subject_alt_name, NULL, NULL);
    ret = get_email(X509_REQ_get_subject_name(x), gens);
    sk_GENERAL_NAME_pop_free(gens, GENERAL_NAME_free);
    sk_X509_EXTENSION_pop_free(exts, X509_EXTENSION_free);
    return ret;
}