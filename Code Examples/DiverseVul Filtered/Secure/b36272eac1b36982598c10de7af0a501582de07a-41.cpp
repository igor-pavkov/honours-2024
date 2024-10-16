void sha256_update(SHA256CTX c, const void *data, unsigned long len)
{
    EVP_DigestUpdate(c, data, len);
}