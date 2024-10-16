CMS_ContentInfo *CMS_digest_create(BIO *in, const EVP_MD *md,
                                   unsigned int flags)
{
    CMS_ContentInfo *cms;
    if (!md)
        md = EVP_sha1();
    cms = cms_DigestedData_create(md);
    if (!cms)
        return NULL;

    if (!(flags & CMS_DETACHED))
        CMS_set_detached(cms, 0);

    if ((flags & CMS_STREAM) || CMS_final(cms, in, NULL, flags))
        return cms;

    CMS_ContentInfo_free(cms);
    return NULL;
}