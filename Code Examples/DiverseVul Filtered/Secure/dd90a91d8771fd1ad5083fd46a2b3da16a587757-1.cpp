static int cms_signerinfo_verify_cert(CMS_SignerInfo *si,
                                      X509_STORE *store,
                                      STACK_OF(X509) *certs,
                                      STACK_OF(X509_CRL) *crls,
                                      unsigned int flags)
{
    X509_STORE_CTX ctx;
    X509 *signer;
    int i, j, r = 0;
    CMS_SignerInfo_get0_algs(si, NULL, &signer, NULL, NULL);
    if (!X509_STORE_CTX_init(&ctx, store, signer, certs)) {
        CMSerr(CMS_F_CMS_SIGNERINFO_VERIFY_CERT, CMS_R_STORE_INIT_ERROR);
        goto err;
    }
    X509_STORE_CTX_set_default(&ctx, "smime_sign");
    if (crls)
        X509_STORE_CTX_set0_crls(&ctx, crls);

    i = X509_verify_cert(&ctx);
    if (i <= 0) {
        j = X509_STORE_CTX_get_error(&ctx);
        CMSerr(CMS_F_CMS_SIGNERINFO_VERIFY_CERT,
               CMS_R_CERTIFICATE_VERIFY_ERROR);
        ERR_add_error_data(2, "Verify error:",
                           X509_verify_cert_error_string(j));
        goto err;
    }
    r = 1;
 err:
    X509_STORE_CTX_cleanup(&ctx);
    return r;

}