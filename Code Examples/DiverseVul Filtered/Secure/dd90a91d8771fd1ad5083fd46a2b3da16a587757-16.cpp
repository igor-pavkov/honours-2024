int CMS_data(CMS_ContentInfo *cms, BIO *out, unsigned int flags)
{
    BIO *cont;
    int r;
    if (OBJ_obj2nid(CMS_get0_type(cms)) != NID_pkcs7_data) {
        CMSerr(CMS_F_CMS_DATA, CMS_R_TYPE_NOT_DATA);
        return 0;
    }
    cont = CMS_dataInit(cms, NULL);
    if (!cont)
        return 0;
    r = cms_copy_content(out, cont, flags);
    BIO_free_all(cont);
    return r;
}