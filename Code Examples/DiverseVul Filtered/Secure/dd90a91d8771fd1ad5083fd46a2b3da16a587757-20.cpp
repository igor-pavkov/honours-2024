int CMS_verify_receipt(CMS_ContentInfo *rcms, CMS_ContentInfo *ocms,
                       STACK_OF(X509) *certs,
                       X509_STORE *store, unsigned int flags)
{
    int r;
    flags &= ~(CMS_DETACHED | CMS_TEXT);
    r = CMS_verify(rcms, certs, store, NULL, NULL, flags);
    if (r <= 0)
        return r;
    return cms_Receipt_verify(rcms, ocms);
}