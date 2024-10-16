void sha512_update(SHA512CTX c, const void *data, unsigned long len)
{
    EVP_DigestUpdate(c, data, len);
}