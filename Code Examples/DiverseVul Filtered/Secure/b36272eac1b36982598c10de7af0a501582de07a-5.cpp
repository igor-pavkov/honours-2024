void md5_update(MD5CTX c, const void *data, unsigned long len)
{
    EVP_DigestUpdate(c, data, len);
}