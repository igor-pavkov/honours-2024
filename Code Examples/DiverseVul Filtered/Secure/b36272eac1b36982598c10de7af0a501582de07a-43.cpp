SHA384CTX sha384_init(void)
{
    int rc;
    SHA384CTX c = EVP_MD_CTX_create();
    if (c == NULL) {
        return NULL;
    }
    EVP_MD_CTX_init(c);
    rc = EVP_DigestInit_ex(c, EVP_sha384(), NULL);
    if (rc == 0) {
        EVP_MD_CTX_destroy(c);
        c = NULL;
    }
    return c;
}