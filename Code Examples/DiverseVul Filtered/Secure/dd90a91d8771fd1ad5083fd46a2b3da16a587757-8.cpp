CMS_ContentInfo *CMS_encrypt(STACK_OF(X509) *certs, BIO *data,
                             const EVP_CIPHER *cipher, unsigned int flags)
{
    CMS_ContentInfo *cms;
    int i;
    X509 *recip;
    cms = CMS_EnvelopedData_create(cipher);
    if (!cms)
        goto merr;
    for (i = 0; i < sk_X509_num(certs); i++) {
        recip = sk_X509_value(certs, i);
        if (!CMS_add1_recipient_cert(cms, recip, flags)) {
            CMSerr(CMS_F_CMS_ENCRYPT, CMS_R_RECIPIENT_ERROR);
            goto err;
        }
    }

    if (!(flags & CMS_DETACHED))
        CMS_set_detached(cms, 0);

    if ((flags & (CMS_STREAM | CMS_PARTIAL))
        || CMS_final(cms, data, NULL, flags))
        return cms;
    else
        goto err;

 merr:
    CMSerr(CMS_F_CMS_ENCRYPT, ERR_R_MALLOC_FAILURE);
 err:
    if (cms)
        CMS_ContentInfo_free(cms);
    return NULL;
}