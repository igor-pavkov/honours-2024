MD5CTX md5_init(void)
{
    int rc;
    MD5CTX c = EVP_MD_CTX_create();
    if (c == NULL) {
        return NULL;
    }
    EVP_MD_CTX_init(c);
    rc = EVP_DigestInit_ex(c, EVP_md5(), NULL);
    if(rc == 0) {
        EVP_MD_CTX_destroy(c);
        c = NULL;
    }
    return c;
}