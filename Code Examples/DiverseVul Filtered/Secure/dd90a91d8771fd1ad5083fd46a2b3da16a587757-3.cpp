CMS_ContentInfo *CMS_sign(X509 *signcert, EVP_PKEY *pkey,
                          STACK_OF(X509) *certs, BIO *data,
                          unsigned int flags)
{
    CMS_ContentInfo *cms;
    int i;

    cms = CMS_ContentInfo_new();
    if (!cms || !CMS_SignedData_init(cms))
        goto merr;

    if (pkey && !CMS_add1_signer(cms, signcert, pkey, NULL, flags)) {
        CMSerr(CMS_F_CMS_SIGN, CMS_R_ADD_SIGNER_ERROR);
        goto err;
    }

    for (i = 0; i < sk_X509_num(certs); i++) {
        X509 *x = sk_X509_value(certs, i);
        if (!CMS_add1_cert(cms, x))
            goto merr;
    }

    if (!(flags & CMS_DETACHED))
        CMS_set_detached(cms, 0);

    if ((flags & (CMS_STREAM | CMS_PARTIAL))
        || CMS_final(cms, data, NULL, flags))
        return cms;
    else
        goto err;

 merr:
    CMSerr(CMS_F_CMS_SIGN, ERR_R_MALLOC_FAILURE);

 err:
    if (cms)
        CMS_ContentInfo_free(cms);
    return NULL;
}