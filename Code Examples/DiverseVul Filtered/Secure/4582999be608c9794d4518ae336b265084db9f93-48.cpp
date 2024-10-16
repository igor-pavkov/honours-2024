void ssl_set_own_cert_alt( ssl_context *ssl, x509_cert *own_cert,
                           void *rsa_key,
                           rsa_decrypt_func rsa_decrypt,
                           rsa_sign_func rsa_sign,
                           rsa_key_len_func rsa_key_len )
{
    ssl->own_cert   = own_cert;
    ssl->rsa_key    = rsa_key;
    ssl->rsa_decrypt = rsa_decrypt;
    ssl->rsa_sign = rsa_sign;
    ssl->rsa_key_len = rsa_key_len;
}