static int get_crl_sk(X509_STORE_CTX *ctx, X509_CRL **pcrl,
                      X509_NAME *nm, STACK_OF(X509_CRL) *crls)
{
    int i;
    X509_CRL *crl, *best_crl = NULL;
    for (i = 0; i < sk_X509_CRL_num(crls); i++) {
        crl = sk_X509_CRL_value(crls, i);
        if (X509_NAME_cmp(nm, X509_CRL_get_issuer(crl)))
            continue;
        if (check_crl_time(ctx, crl, 0)) {
            *pcrl = crl;
            CRYPTO_add(&crl->references, 1, CRYPTO_LOCK_X509);
            return 1;
        }
        best_crl = crl;
    }
    if (best_crl) {
        *pcrl = best_crl;
        CRYPTO_add(&best_crl->references, 1, CRYPTO_LOCK_X509);
    }

    return 0;
}