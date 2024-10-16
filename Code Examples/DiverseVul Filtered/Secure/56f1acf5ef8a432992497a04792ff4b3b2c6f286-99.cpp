void ssl_clear_hash_ctx(EVP_MD_CTX **hash)
{

    if (*hash)
        EVP_MD_CTX_destroy(*hash);
    *hash = NULL;
}