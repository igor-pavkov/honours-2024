int CMS_decrypt_set1_key(CMS_ContentInfo *cms,
                         unsigned char *key, size_t keylen,
                         unsigned char *id, size_t idlen)
{
    STACK_OF(CMS_RecipientInfo) *ris;
    CMS_RecipientInfo *ri;
    int i, r;
    ris = CMS_get0_RecipientInfos(cms);
    for (i = 0; i < sk_CMS_RecipientInfo_num(ris); i++) {
        ri = sk_CMS_RecipientInfo_value(ris, i);
        if (CMS_RecipientInfo_type(ri) != CMS_RECIPINFO_KEK)
            continue;

        /*
         * If we have an id try matching RecipientInfo otherwise try them
         * all.
         */
        if (!id || (CMS_RecipientInfo_kekri_id_cmp(ri, id, idlen) == 0)) {
            CMS_RecipientInfo_set0_key(ri, key, keylen);
            r = CMS_RecipientInfo_decrypt(cms, ri);
            CMS_RecipientInfo_set0_key(ri, NULL, 0);
            if (r > 0)
                return 1;
            if (id) {
                CMSerr(CMS_F_CMS_DECRYPT_SET1_KEY, CMS_R_DECRYPT_ERROR);
                return 0;
            }
            ERR_clear_error();
        }
    }

    CMSerr(CMS_F_CMS_DECRYPT_SET1_KEY, CMS_R_NO_MATCHING_RECIPIENT);
    return 0;

}