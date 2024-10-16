static int cert_crl(X509_STORE_CTX *ctx, X509_CRL *crl, X509 *x)
{
    int idx, ok;
    X509_REVOKED rtmp;
    STACK_OF(X509_EXTENSION) *exts;
    X509_EXTENSION *ext;
    /* Look for serial number of certificate in CRL */
    rtmp.serialNumber = X509_get_serialNumber(x);
    /*
     * Sort revoked into serial number order if not already sorted. Do this
     * under a lock to avoid race condition.
     */
    if (!sk_X509_REVOKED_is_sorted(crl->crl->revoked)) {
        CRYPTO_w_lock(CRYPTO_LOCK_X509_CRL);
        sk_X509_REVOKED_sort(crl->crl->revoked);
        CRYPTO_w_unlock(CRYPTO_LOCK_X509_CRL);
    }
    idx = sk_X509_REVOKED_find(crl->crl->revoked, &rtmp);
    /*
     * If found assume revoked: want something cleverer than this to handle
     * entry extensions in V2 CRLs.
     */
    if (idx >= 0) {
        ctx->error = X509_V_ERR_CERT_REVOKED;
        ok = ctx->verify_cb(0, ctx);
        if (!ok)
            return 0;
    }

    if (ctx->param->flags & X509_V_FLAG_IGNORE_CRITICAL)
        return 1;

    /*
     * See if we have any critical CRL extensions: since we currently don't
     * handle any CRL extensions the CRL must be rejected. This code
     * accesses the X509_CRL structure directly: applications shouldn't do
     * this.
     */

    exts = crl->crl->extensions;

    for (idx = 0; idx < sk_X509_EXTENSION_num(exts); idx++) {
        ext = sk_X509_EXTENSION_value(exts, idx);
        if (ext->critical > 0) {
            ctx->error = X509_V_ERR_UNHANDLED_CRITICAL_CRL_EXTENSION;
            ok = ctx->verify_cb(0, ctx);
            if (!ok)
                return 0;
            break;
        }
    }
    return 1;
}