int CMS_EncryptedData_decrypt(CMS_ContentInfo *cms,
                              const unsigned char *key, size_t keylen,
                              BIO *dcont, BIO *out, unsigned int flags)
{
    BIO *cont;
    int r;
    if (OBJ_obj2nid(CMS_get0_type(cms)) != NID_pkcs7_encrypted) {
        CMSerr(CMS_F_CMS_ENCRYPTEDDATA_DECRYPT,
               CMS_R_TYPE_NOT_ENCRYPTED_DATA);
        return 0;
    }

    if (!dcont && !check_content(cms))
        return 0;

    if (CMS_EncryptedData_set1_key(cms, NULL, key, keylen) <= 0)
        return 0;
    cont = CMS_dataInit(cms, dcont);
    if (!cont)
        return 0;
    r = cms_copy_content(out, cont, flags);
    do_free_upto(cont, dcont);
    return r;
}