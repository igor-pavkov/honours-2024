CMS_ContentInfo *CMS_EncryptedData_encrypt(BIO *in, const EVP_CIPHER *cipher,
                                           const unsigned char *key,
                                           size_t keylen, unsigned int flags)
{
    CMS_ContentInfo *cms;
    if (!cipher) {
        CMSerr(CMS_F_CMS_ENCRYPTEDDATA_ENCRYPT, CMS_R_NO_CIPHER);
        return NULL;
    }
    cms = CMS_ContentInfo_new();
    if (!cms)
        return NULL;
    if (!CMS_EncryptedData_set1_key(cms, cipher, key, keylen))
        return NULL;

    if (!(flags & CMS_DETACHED))
        CMS_set_detached(cms, 0);

    if ((flags & (CMS_STREAM | CMS_PARTIAL))
        || CMS_final(cms, in, NULL, flags))
        return cms;

    CMS_ContentInfo_free(cms);
    return NULL;
}