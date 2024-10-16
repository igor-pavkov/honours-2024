int CMS_uncompress(CMS_ContentInfo *cms, BIO *dcont, BIO *out,
                   unsigned int flags)
{
    CMSerr(CMS_F_CMS_UNCOMPRESS, CMS_R_UNSUPPORTED_COMPRESSION_ALGORITHM);
    return 0;
}