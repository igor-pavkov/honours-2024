CMS_ContentInfo *CMS_sign_receipt(CMS_SignerInfo *si,
                                  X509 *signcert, EVP_PKEY *pkey,
                                  STACK_OF(X509) *certs, unsigned int flags)
{
    CMS_SignerInfo *rct_si;
    CMS_ContentInfo *cms = NULL;
    ASN1_OCTET_STRING **pos, *os;
    BIO *rct_cont = NULL;
    int r = 0;

    flags &= ~(CMS_STREAM | CMS_TEXT);
    /* Not really detached but avoids content being allocated */
    flags |= CMS_PARTIAL | CMS_BINARY | CMS_DETACHED;
    if (!pkey || !signcert) {
        CMSerr(CMS_F_CMS_SIGN_RECEIPT, CMS_R_NO_KEY_OR_CERT);
        return NULL;
    }

    /* Initialize signed data */

    cms = CMS_sign(NULL, NULL, certs, NULL, flags);
    if (!cms)
        goto err;

    /* Set inner content type to signed receipt */
    if (!CMS_set1_eContentType(cms, OBJ_nid2obj(NID_id_smime_ct_receipt)))
        goto err;

    rct_si = CMS_add1_signer(cms, signcert, pkey, NULL, flags);
    if (!rct_si) {
        CMSerr(CMS_F_CMS_SIGN_RECEIPT, CMS_R_ADD_SIGNER_ERROR);
        goto err;
    }

    os = cms_encode_Receipt(si);

    if (!os)
        goto err;

    /* Set content to digest */
    rct_cont = BIO_new_mem_buf(os->data, os->length);
    if (!rct_cont)
        goto err;

    /* Add msgSigDigest attribute */

    if (!cms_msgSigDigest_add1(rct_si, si))
        goto err;

    /* Finalize structure */
    if (!CMS_final(cms, rct_cont, NULL, flags))
        goto err;

    /* Set embedded content */
    pos = CMS_get0_content(cms);
    *pos = os;

    r = 1;

 err:
    if (rct_cont)
        BIO_free(rct_cont);
    if (r)
        return cms;
    CMS_ContentInfo_free(cms);
    return NULL;

}