int PKCS7_dataFinal(PKCS7 *p7, BIO *bio)
{
    int ret = 0;
    int i, j;
    BIO *btmp;
    PKCS7_SIGNER_INFO *si;
    EVP_MD_CTX *mdc, ctx_tmp;
    STACK_OF(X509_ATTRIBUTE) *sk;
    STACK_OF(PKCS7_SIGNER_INFO) *si_sk = NULL;
    ASN1_OCTET_STRING *os = NULL;

    if (p7 == NULL) {
        PKCS7err(PKCS7_F_PKCS7_DATAFINAL, PKCS7_R_INVALID_NULL_POINTER);
        return 0;
    }

    if (p7->d.ptr == NULL) {
        PKCS7err(PKCS7_F_PKCS7_DATAFINAL, PKCS7_R_NO_CONTENT);
        return 0;
    }

    EVP_MD_CTX_init(&ctx_tmp);
    i = OBJ_obj2nid(p7->type);
    p7->state = PKCS7_S_HEADER;

    switch (i) {
    case NID_pkcs7_data:
        os = p7->d.data;
        break;
    case NID_pkcs7_signedAndEnveloped:
        /* XXXXXXXXXXXXXXXX */
        si_sk = p7->d.signed_and_enveloped->signer_info;
        os = p7->d.signed_and_enveloped->enc_data->enc_data;
        if (!os) {
            os = ASN1_OCTET_STRING_new();
            if (!os) {
                PKCS7err(PKCS7_F_PKCS7_DATAFINAL, ERR_R_MALLOC_FAILURE);
                goto err;
            }
            p7->d.signed_and_enveloped->enc_data->enc_data = os;
        }
        break;
    case NID_pkcs7_enveloped:
        /* XXXXXXXXXXXXXXXX */
        os = p7->d.enveloped->enc_data->enc_data;
        if (!os) {
            os = ASN1_OCTET_STRING_new();
            if (!os) {
                PKCS7err(PKCS7_F_PKCS7_DATAFINAL, ERR_R_MALLOC_FAILURE);
                goto err;
            }
            p7->d.enveloped->enc_data->enc_data = os;
        }
        break;
    case NID_pkcs7_signed:
        si_sk = p7->d.sign->signer_info;
        os = PKCS7_get_octet_string(p7->d.sign->contents);
        /* If detached data then the content is excluded */
        if (PKCS7_type_is_data(p7->d.sign->contents) && p7->detached) {
            ASN1_OCTET_STRING_free(os);
            os = NULL;
            p7->d.sign->contents->d.data = NULL;
        }
        break;

    case NID_pkcs7_digest:
        os = PKCS7_get_octet_string(p7->d.digest->contents);
        /* If detached data then the content is excluded */
        if (PKCS7_type_is_data(p7->d.digest->contents) && p7->detached) {
            ASN1_OCTET_STRING_free(os);
            os = NULL;
            p7->d.digest->contents->d.data = NULL;
        }
        break;

    default:
        PKCS7err(PKCS7_F_PKCS7_DATAFINAL, PKCS7_R_UNSUPPORTED_CONTENT_TYPE);
        goto err;
    }

    if (si_sk != NULL) {
        for (i = 0; i < sk_PKCS7_SIGNER_INFO_num(si_sk); i++) {
            si = sk_PKCS7_SIGNER_INFO_value(si_sk, i);
            if (si->pkey == NULL)
                continue;

            j = OBJ_obj2nid(si->digest_alg->algorithm);

            btmp = bio;

            btmp = PKCS7_find_digest(&mdc, btmp, j);

            if (btmp == NULL)
                goto err;

            /*
             * We now have the EVP_MD_CTX, lets do the signing.
             */
            if (!EVP_MD_CTX_copy_ex(&ctx_tmp, mdc))
                goto err;

            sk = si->auth_attr;

            /*
             * If there are attributes, we add the digest attribute and only
             * sign the attributes
             */
            if (sk_X509_ATTRIBUTE_num(sk) > 0) {
                if (!do_pkcs7_signed_attrib(si, &ctx_tmp))
                    goto err;
            } else {
                unsigned char *abuf = NULL;
                unsigned int abuflen;
                abuflen = EVP_PKEY_size(si->pkey);
                abuf = OPENSSL_malloc(abuflen);
                if (!abuf)
                    goto err;

                if (!EVP_SignFinal(&ctx_tmp, abuf, &abuflen, si->pkey)) {
                    PKCS7err(PKCS7_F_PKCS7_DATAFINAL, ERR_R_EVP_LIB);
                    goto err;
                }
                ASN1_STRING_set0(si->enc_digest, abuf, abuflen);
            }
        }
    } else if (i == NID_pkcs7_digest) {
        unsigned char md_data[EVP_MAX_MD_SIZE];
        unsigned int md_len;
        if (!PKCS7_find_digest(&mdc, bio,
                               OBJ_obj2nid(p7->d.digest->md->algorithm)))
            goto err;
        if (!EVP_DigestFinal_ex(mdc, md_data, &md_len))
            goto err;
        ASN1_OCTET_STRING_set(p7->d.digest->digest, md_data, md_len);
    }

    if (!PKCS7_is_detached(p7)) {
        /*
         * NOTE(emilia): I think we only reach os == NULL here because detached
         * digested data support is broken.
         */
        if (os == NULL)
            goto err;
        if (!(os->flags & ASN1_STRING_FLAG_NDEF)) {
            char *cont;
            long contlen;
            btmp = BIO_find_type(bio, BIO_TYPE_MEM);
            if (btmp == NULL) {
                PKCS7err(PKCS7_F_PKCS7_DATAFINAL, PKCS7_R_UNABLE_TO_FIND_MEM_BIO);
                goto err;
            }
            contlen = BIO_get_mem_data(btmp, &cont);
            /*
             * Mark the BIO read only then we can use its copy of the data
             * instead of making an extra copy.
             */
            BIO_set_flags(btmp, BIO_FLAGS_MEM_RDONLY);
            BIO_set_mem_eof_return(btmp, 0);
            ASN1_STRING_set0(os, (unsigned char *)cont, contlen);
        }
    }
    ret = 1;
 err:
    EVP_MD_CTX_cleanup(&ctx_tmp);
    return (ret);
}