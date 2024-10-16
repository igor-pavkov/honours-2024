EVP_MD_CTX *ssl_replace_hash(EVP_MD_CTX **hash, const EVP_MD *md)
{
    ssl_clear_hash_ctx(hash);
    *hash = EVP_MD_CTX_create();
    if (*hash == NULL || (md && EVP_DigestInit_ex(*hash, md, NULL) <= 0)) {
        EVP_MD_CTX_destroy(*hash);
        *hash = NULL;
        return NULL;
    }
    return *hash;
}