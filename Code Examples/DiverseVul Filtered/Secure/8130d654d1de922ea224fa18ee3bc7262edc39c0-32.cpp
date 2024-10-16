unsigned long X509_NAME_hash_old(const X509_NAME *x)
{
    EVP_MD *md5 = EVP_MD_fetch(NULL, OSSL_DIGEST_NAME_MD5, "-fips");
    EVP_MD_CTX *md_ctx = EVP_MD_CTX_new();
    unsigned long ret = 0;
    unsigned char md[16];

    if (md5 == NULL || md_ctx == NULL)
        goto end;

    /* Make sure X509_NAME structure contains valid cached encoding */
    i2d_X509_NAME(x, NULL);
    if (EVP_DigestInit_ex(md_ctx, md5, NULL)
        && EVP_DigestUpdate(md_ctx, x->bytes->data, x->bytes->length)
        && EVP_DigestFinal_ex(md_ctx, md, NULL))
        ret = (((unsigned long)md[0]) | ((unsigned long)md[1] << 8L) |
               ((unsigned long)md[2] << 16L) | ((unsigned long)md[3] << 24L)
            ) & 0xffffffffL;

 end:
    EVP_MD_CTX_free(md_ctx);
    EVP_MD_free(md5);

    return ret;
}