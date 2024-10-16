STACK_OF(X509) *X509_chain_up_ref(STACK_OF(X509) *chain)
{
    STACK_OF(X509) *ret = sk_X509_dup(chain);
    int i;

    if (ret == NULL)
        return NULL;
    for (i = 0; i < sk_X509_num(ret); i++) {
        X509 *x = sk_X509_value(ret, i);

        if (!X509_up_ref(x))
            goto err;
    }
    return ret;

 err:
    while (i-- > 0)
        X509_free(sk_X509_value(ret, i));
    sk_X509_free(ret);
    return NULL;
}