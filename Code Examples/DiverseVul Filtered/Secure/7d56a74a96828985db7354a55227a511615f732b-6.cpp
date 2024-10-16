static int rc4_hmac_md5_get_params(OSSL_PARAM params[])
{
    return ossl_cipher_generic_get_params(params, RC4_HMAC_MD5_MODE,
                                          RC4_HMAC_MD5_FLAGS,
                                          RC4_HMAC_MD5_KEY_BITS,
                                          RC4_HMAC_MD5_BLOCK_BITS,
                                          RC4_HMAC_MD5_IV_BITS);
}