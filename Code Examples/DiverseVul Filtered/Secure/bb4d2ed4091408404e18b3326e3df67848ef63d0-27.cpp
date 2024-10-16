STACK_OF(OPENSSL_STRING) *X509_get1_email(X509 *x)
{
    GENERAL_NAMES *gens;
    STACK_OF(OPENSSL_STRING) *ret;

    gens = X509_get_ext_d2i(x, NID_subject_alt_name, NULL, NULL);
    ret = get_email(X509_get_subject_name(x), gens);
    sk_GENERAL_NAME_pop_free(gens, GENERAL_NAME_free);
    return ret;
}