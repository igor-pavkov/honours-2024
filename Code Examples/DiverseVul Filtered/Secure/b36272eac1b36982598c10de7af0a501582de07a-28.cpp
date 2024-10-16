void sha384_update(SHA384CTX c, const void *data, unsigned long len)
{
    EVP_DigestUpdate(c, data, len);
}