int CMS_decrypt_set1_password(CMS_ContentInfo *cms,
                              unsigned char *pass, ossl_ssize_t passlen)
{
    STACK_OF(CMS_RecipientInfo) *ris;
    CMS_RecipientInfo *ri;
    int i, r;
    ris = CMS_get0_RecipientInfos(cms);
    for (i = 0; i < sk_CMS_RecipientInfo_num(ris); i++) {
        ri = sk_CMS_RecipientInfo_value(ris, i);
        if (CMS_RecipientInfo_type(ri) != CMS_RECIPINFO_PASS)
            continue;
        CMS_RecipientInfo_set0_password(ri, pass, passlen);
        r = CMS_RecipientInfo_decrypt(cms, ri);
        CMS_RecipientInfo_set0_password(ri, NULL, 0);
        if (r > 0)
            return 1;
    }

    CMSerr(CMS_F_CMS_DECRYPT_SET1_PASSWORD, CMS_R_NO_MATCHING_RECIPIENT);
    return 0;

}