unsigned long X509_NAME_hash_ex(const X509_NAME *x, OSSL_LIB_CTX *libctx,
                                const char *propq, int *ok)
{
    unsigned long ret = 0;
    unsigned char md[SHA_DIGEST_LENGTH];
    EVP_MD *sha1 = EVP_MD_fetch(libctx, "SHA1", propq);

    /* Make sure X509_NAME structure contains valid cached encoding */
    i2d_X509_NAME(x, NULL);
    if (ok != NULL)
        *ok = 0;
    if (sha1 != NULL
        && EVP_Digest(x->canon_enc, x->canon_enclen, md, NULL, sha1, NULL)) {
        ret = (((unsigned long)md[0]) | ((unsigned long)md[1] << 8L) |
               ((unsigned long)md[2] << 16L) | ((unsigned long)md[3] << 24L)
               ) & 0xffffffffL;
        if (ok != NULL)
            *ok = 1;
    }
    EVP_MD_free(sha1);
    return ret;
}