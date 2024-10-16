static int rsa_item_sign(EVP_MD_CTX *ctx, const ASN1_ITEM *it, void *asn,
                         X509_ALGOR *alg1, X509_ALGOR *alg2,
                         ASN1_BIT_STRING *sig)
{
    int pad_mode;
    EVP_PKEY_CTX *pkctx = ctx->pctx;
    if (EVP_PKEY_CTX_get_rsa_padding(pkctx, &pad_mode) <= 0)
        return 0;
    if (pad_mode == RSA_PKCS1_PADDING)
        return 2;
    if (pad_mode == RSA_PKCS1_PSS_PADDING) {
        const EVP_MD *sigmd, *mgf1md;
        RSA_PSS_PARAMS *pss = NULL;
        X509_ALGOR *mgf1alg = NULL;
        ASN1_STRING *os1 = NULL, *os2 = NULL;
        EVP_PKEY *pk = EVP_PKEY_CTX_get0_pkey(pkctx);
        int saltlen, rv = 0;
        sigmd = EVP_MD_CTX_md(ctx);
        if (EVP_PKEY_CTX_get_rsa_mgf1_md(pkctx, &mgf1md) <= 0)
            goto err;
        if (!EVP_PKEY_CTX_get_rsa_pss_saltlen(pkctx, &saltlen))
            goto err;
        if (saltlen == -1)
            saltlen = EVP_MD_size(sigmd);
        else if (saltlen == -2) {
            saltlen = EVP_PKEY_size(pk) - EVP_MD_size(sigmd) - 2;
            if (((EVP_PKEY_bits(pk) - 1) & 0x7) == 0)
                saltlen--;
        }
        pss = RSA_PSS_PARAMS_new();
        if (!pss)
            goto err;
        if (saltlen != 20) {
            pss->saltLength = ASN1_INTEGER_new();
            if (!pss->saltLength)
                goto err;
            if (!ASN1_INTEGER_set(pss->saltLength, saltlen))
                goto err;
        }
        if (EVP_MD_type(sigmd) != NID_sha1) {
            pss->hashAlgorithm = X509_ALGOR_new();
            if (!pss->hashAlgorithm)
                goto err;
            X509_ALGOR_set_md(pss->hashAlgorithm, sigmd);
        }
        if (EVP_MD_type(mgf1md) != NID_sha1) {
            ASN1_STRING *stmp = NULL;
            /* need to embed algorithm ID inside another */
            mgf1alg = X509_ALGOR_new();
            X509_ALGOR_set_md(mgf1alg, mgf1md);
            if (!ASN1_item_pack(mgf1alg, ASN1_ITEM_rptr(X509_ALGOR), &stmp))
                 goto err;
            pss->maskGenAlgorithm = X509_ALGOR_new();
            if (!pss->maskGenAlgorithm)
                goto err;
            X509_ALGOR_set0(pss->maskGenAlgorithm,
                            OBJ_nid2obj(NID_mgf1), V_ASN1_SEQUENCE, stmp);
        }
        /* Finally create string with pss parameter encoding. */
        if (!ASN1_item_pack(pss, ASN1_ITEM_rptr(RSA_PSS_PARAMS), &os1))
             goto err;
        if (alg2) {
            os2 = ASN1_STRING_dup(os1);
            if (!os2)
                goto err;
            X509_ALGOR_set0(alg2, OBJ_nid2obj(NID_rsassaPss),
                            V_ASN1_SEQUENCE, os2);
        }
        X509_ALGOR_set0(alg1, OBJ_nid2obj(NID_rsassaPss),
                        V_ASN1_SEQUENCE, os1);
        os1 = os2 = NULL;
        rv = 3;
 err:
        if (mgf1alg)
            X509_ALGOR_free(mgf1alg);
        if (pss)
            RSA_PSS_PARAMS_free(pss);
        if (os1)
            ASN1_STRING_free(os1);
        return rv;

    }
    return 2;
}