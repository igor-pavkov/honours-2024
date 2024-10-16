int CMS_final(CMS_ContentInfo *cms, BIO *data, BIO *dcont, unsigned int flags)
{
    BIO *cmsbio;
    int ret = 0;
    if (!(cmsbio = CMS_dataInit(cms, dcont))) {
        CMSerr(CMS_F_CMS_FINAL, ERR_R_MALLOC_FAILURE);
        return 0;
    }

    SMIME_crlf_copy(data, cmsbio, flags);

    (void)BIO_flush(cmsbio);

    if (!CMS_dataFinal(cms, cmsbio)) {
        CMSerr(CMS_F_CMS_FINAL, CMS_R_CMS_DATAFINAL_ERROR);
        goto err;
    }

    ret = 1;

 err:
    do_free_upto(cmsbio, dcont);

    return ret;

}