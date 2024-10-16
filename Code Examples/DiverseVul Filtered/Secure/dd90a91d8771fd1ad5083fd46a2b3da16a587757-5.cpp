CMS_ContentInfo *CMS_data_create(BIO *in, unsigned int flags)
{
    CMS_ContentInfo *cms;
    cms = cms_Data_create();
    if (!cms)
        return NULL;

    if ((flags & CMS_STREAM) || CMS_final(cms, in, NULL, flags))
        return cms;

    CMS_ContentInfo_free(cms);

    return NULL;
}