int X509_STORE_CTX_purpose_inherit(X509_STORE_CTX *ctx, int def_purpose,
                                   int purpose, int trust)
{
    int idx;
    /* If purpose not set use default */
    if (!purpose)
        purpose = def_purpose;
    /* If we have a purpose then check it is valid */
    if (purpose) {
        X509_PURPOSE *ptmp;
        idx = X509_PURPOSE_get_by_id(purpose);
        if (idx == -1) {
            X509err(X509_F_X509_STORE_CTX_PURPOSE_INHERIT,
                    X509_R_UNKNOWN_PURPOSE_ID);
            return 0;
        }
        ptmp = X509_PURPOSE_get0(idx);
        if (ptmp->trust == X509_TRUST_DEFAULT) {
            idx = X509_PURPOSE_get_by_id(def_purpose);
            if (idx == -1) {
                X509err(X509_F_X509_STORE_CTX_PURPOSE_INHERIT,
                        X509_R_UNKNOWN_PURPOSE_ID);
                return 0;
            }
            ptmp = X509_PURPOSE_get0(idx);
        }
        /* If trust not set then get from purpose default */
        if (!trust)
            trust = ptmp->trust;
    }
    if (trust) {
        idx = X509_TRUST_get_by_id(trust);
        if (idx == -1) {
            X509err(X509_F_X509_STORE_CTX_PURPOSE_INHERIT,
                    X509_R_UNKNOWN_TRUST_ID);
            return 0;
        }
    }

    if (purpose && !ctx->param->purpose)
        ctx->param->purpose = purpose;
    if (trust && !ctx->param->trust)
        ctx->param->trust = trust;
    return 1;
}