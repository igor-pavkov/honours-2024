static int cert_crl(X509_STORE_CTX *ctx, X509_CRL *crl, X509 *x)
{
    int ok;
    X509_REVOKED *rev;
    /*
     * The rules changed for this... previously if a CRL contained unhandled
     * critical extensions it could still be used to indicate a certificate
     * was revoked. This has since been changed since critical extension can
     * change the meaning of CRL entries.
     */
    if (!(ctx->param->flags & X509_V_FLAG_IGNORE_CRITICAL)
        && (crl->flags & EXFLAG_CRITICAL)) {
        ctx->error = X509_V_ERR_UNHANDLED_CRITICAL_CRL_EXTENSION;
        ok = ctx->verify_cb(0, ctx);
        if (!ok)
            return 0;
    }
    /*
     * Look for serial number of certificate in CRL If found make sure reason
     * is not removeFromCRL.
     */
    if (X509_CRL_get0_by_cert(crl, &rev, x)) {
        if (rev->reason == CRL_REASON_REMOVE_FROM_CRL)
            return 2;
        ctx->error = X509_V_ERR_CERT_REVOKED;
        ok = ctx->verify_cb(0, ctx);
        if (!ok)
            return 0;
    }

    return 1;
}