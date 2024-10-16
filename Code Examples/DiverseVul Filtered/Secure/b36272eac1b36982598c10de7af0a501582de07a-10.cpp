void sha1_update(SHACTX c, const void *data, unsigned long len)
{
    EVP_DigestUpdate(c, data, len);
}