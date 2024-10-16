const EVP_CIPHER *EVP_rc4_hmac_md5(void)
{
    return (&r4_hmac_md5_cipher);
}