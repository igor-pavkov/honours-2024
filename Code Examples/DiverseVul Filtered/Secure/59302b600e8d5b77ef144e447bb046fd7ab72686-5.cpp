int PKCS7_SIGNER_INFO_sign(PKCS7_SIGNER_INFO *si)
{
    EVP_MD_CTX mctx;
    EVP_PKEY_CTX *pctx;
    unsigned char *abuf = NULL;
    int alen;
    size_t siglen;
    const EVP_MD *md = NULL;

    md = EVP_get_digestbyobj(si->digest_alg->algorithm);
    if (md == NULL)
        return 0;

    EVP_MD_CTX_init(&mctx);
    if (EVP_DigestSignInit(&mctx, &pctx, md, NULL, si->pkey) <= 0)
        goto err;

    if (EVP_PKEY_CTX_ctrl(pctx, -1, EVP_PKEY_OP_SIGN,
                          EVP_PKEY_CTRL_PKCS7_SIGN, 0, si) <= 0) {
        PKCS7err(PKCS7_F_PKCS7_SIGNER_INFO_SIGN, PKCS7_R_CTRL_ERROR);
        goto err;
    }

    alen = ASN1_item_i2d((ASN1_VALUE *)si->auth_attr, &abuf,
                         ASN1_ITEM_rptr(PKCS7_ATTR_SIGN));
    if (!abuf)
        goto err;
    if (EVP_DigestSignUpdate(&mctx, abuf, alen) <= 0)
        goto err;
    OPENSSL_free(abuf);
    abuf = NULL;
    if (EVP_DigestSignFinal(&mctx, NULL, &siglen) <= 0)
        goto err;
    abuf = OPENSSL_malloc(siglen);
    if (!abuf)
        goto err;
    if (EVP_DigestSignFinal(&mctx, abuf, &siglen) <= 0)
        goto err;

    if (EVP_PKEY_CTX_ctrl(pctx, -1, EVP_PKEY_OP_SIGN,
                          EVP_PKEY_CTRL_PKCS7_SIGN, 1, si) <= 0) {
        PKCS7err(PKCS7_F_PKCS7_SIGNER_INFO_SIGN, PKCS7_R_CTRL_ERROR);
        goto err;
    }

    EVP_MD_CTX_cleanup(&mctx);

    ASN1_STRING_set0(si->enc_digest, abuf, siglen);

    return 1;

 err:
    OPENSSL_free(abuf);
    EVP_MD_CTX_cleanup(&mctx);
    return 0;

}