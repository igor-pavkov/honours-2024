static int rsa_item_verify(EVP_MD_CTX *ctx, const ASN1_ITEM *it, void *asn,
                           X509_ALGOR *sigalg, ASN1_BIT_STRING *sig,
                           EVP_PKEY *pkey)
{
    int rv = -1;
    int saltlen;
    const EVP_MD *mgf1md = NULL, *md = NULL;
    RSA_PSS_PARAMS *pss;
    X509_ALGOR *maskHash;
    EVP_PKEY_CTX *pkctx;
    /* Sanity check: make sure it is PSS */
    if (OBJ_obj2nid(sigalg->algorithm) != NID_rsassaPss) {
        RSAerr(RSA_F_RSA_ITEM_VERIFY, RSA_R_UNSUPPORTED_SIGNATURE_TYPE);
        return -1;
    }
    /* Decode PSS parameters */
    pss = rsa_pss_decode(sigalg, &maskHash);

    if (pss == NULL) {
        RSAerr(RSA_F_RSA_ITEM_VERIFY, RSA_R_INVALID_PSS_PARAMETERS);
        goto err;
    }
    /* Check mask and lookup mask hash algorithm */
    if (pss->maskGenAlgorithm) {
        if (OBJ_obj2nid(pss->maskGenAlgorithm->algorithm) != NID_mgf1) {
            RSAerr(RSA_F_RSA_ITEM_VERIFY, RSA_R_UNSUPPORTED_MASK_ALGORITHM);
            goto err;
        }
        if (!maskHash) {
            RSAerr(RSA_F_RSA_ITEM_VERIFY, RSA_R_UNSUPPORTED_MASK_PARAMETER);
            goto err;
        }
        mgf1md = EVP_get_digestbyobj(maskHash->algorithm);
        if (mgf1md == NULL) {
            RSAerr(RSA_F_RSA_ITEM_VERIFY, RSA_R_UNKNOWN_MASK_DIGEST);
            goto err;
        }
    } else
        mgf1md = EVP_sha1();

    if (pss->hashAlgorithm) {
        md = EVP_get_digestbyobj(pss->hashAlgorithm->algorithm);
        if (md == NULL) {
            RSAerr(RSA_F_RSA_ITEM_VERIFY, RSA_R_UNKNOWN_PSS_DIGEST);
            goto err;
        }
    } else
        md = EVP_sha1();

    if (pss->saltLength) {
        saltlen = ASN1_INTEGER_get(pss->saltLength);

        /*
         * Could perform more salt length sanity checks but the main RSA
         * routines will trap other invalid values anyway.
         */
        if (saltlen < 0) {
            RSAerr(RSA_F_RSA_ITEM_VERIFY, RSA_R_INVALID_SALT_LENGTH);
            goto err;
        }
    } else
        saltlen = 20;

    /*
     * low-level routines support only trailer field 0xbc (value 1) and
     * PKCS#1 says we should reject any other value anyway.
     */
    if (pss->trailerField && ASN1_INTEGER_get(pss->trailerField) != 1) {
        RSAerr(RSA_F_RSA_ITEM_VERIFY, RSA_R_INVALID_TRAILER);
        goto err;
    }

    /* We have all parameters now set up context */

    if (!EVP_DigestVerifyInit(ctx, &pkctx, md, NULL, pkey))
        goto err;

    if (EVP_PKEY_CTX_set_rsa_padding(pkctx, RSA_PKCS1_PSS_PADDING) <= 0)
        goto err;

    if (EVP_PKEY_CTX_set_rsa_pss_saltlen(pkctx, saltlen) <= 0)
        goto err;

    if (EVP_PKEY_CTX_set_rsa_mgf1_md(pkctx, mgf1md) <= 0)
        goto err;
    /* Carry on */
    rv = 2;

 err:
    RSA_PSS_PARAMS_free(pss);
    if (maskHash)
        X509_ALGOR_free(maskHash);
    return rv;
}