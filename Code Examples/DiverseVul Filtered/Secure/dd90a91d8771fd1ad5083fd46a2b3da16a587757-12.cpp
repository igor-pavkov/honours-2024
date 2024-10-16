int CMS_decrypt_set1_pkey(CMS_ContentInfo *cms, EVP_PKEY *pk, X509 *cert)
{
    STACK_OF(CMS_RecipientInfo) *ris;
    CMS_RecipientInfo *ri;
    int i, r;
    int debug = 0, ri_match = 0;
    ris = CMS_get0_RecipientInfos(cms);
    if (ris)
        debug = cms->d.envelopedData->encryptedContentInfo->debug;
    for (i = 0; i < sk_CMS_RecipientInfo_num(ris); i++) {
        ri = sk_CMS_RecipientInfo_value(ris, i);
        if (CMS_RecipientInfo_type(ri) != CMS_RECIPINFO_TRANS)
            continue;
        ri_match = 1;
        /*
         * If we have a cert try matching RecipientInfo otherwise try them
         * all.
         */
        if (!cert || (CMS_RecipientInfo_ktri_cert_cmp(ri, cert) == 0)) {
            CMS_RecipientInfo_set0_pkey(ri, pk);
            r = CMS_RecipientInfo_decrypt(cms, ri);
            CMS_RecipientInfo_set0_pkey(ri, NULL);
            if (cert) {
                /*
                 * If not debugging clear any error and return success to
                 * avoid leaking of information useful to MMA
                 */
                if (!debug) {
                    ERR_clear_error();
                    return 1;
                }
                if (r > 0)
                    return 1;
                CMSerr(CMS_F_CMS_DECRYPT_SET1_PKEY, CMS_R_DECRYPT_ERROR);
                return 0;
            }
            /*
             * If no cert and not debugging don't leave loop after first
             * successful decrypt. Always attempt to decrypt all recipients
             * to avoid leaking timing of a successful decrypt.
             */
            else if (r > 0 && debug)
                return 1;
        }
    }
    /* If no cert and not debugging always return success */
    if (ri_match && !cert && !debug) {
        ERR_clear_error();
        return 1;
    }

    CMSerr(CMS_F_CMS_DECRYPT_SET1_PKEY, CMS_R_NO_MATCHING_RECIPIENT);
    return 0;

}