static int get_crl(X509_STORE_CTX *ctx, X509_CRL **pcrl, X509 *x)
{
    int ok;
    X509_CRL *crl = NULL;
    X509_OBJECT xobj;
    X509_NAME *nm;
    nm = X509_get_issuer_name(x);
    ok = get_crl_sk(ctx, &crl, nm, ctx->crls);
    if (ok) {
        *pcrl = crl;
        return 1;
    }

    ok = X509_STORE_get_by_subject(ctx, X509_LU_CRL, nm, &xobj);

    if (!ok) {
        /* If we got a near match from get_crl_sk use that */
        if (crl) {
            *pcrl = crl;
            return 1;
        }
        return 0;
    }

    *pcrl = xobj.data.crl;
    if (crl)
        X509_CRL_free(crl);
    return 1;
}