CMS_ContentInfo *CMS_compress(BIO *in, int comp_nid, unsigned int flags)
{
    CMSerr(CMS_F_CMS_COMPRESS, CMS_R_UNSUPPORTED_COMPRESSION_ALGORITHM);
    return NULL;
}