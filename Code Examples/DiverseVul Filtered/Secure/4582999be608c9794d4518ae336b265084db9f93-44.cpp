void ssl_set_own_cert( ssl_context *ssl, x509_cert *own_cert,
                       rsa_context *rsa_key )
{
    ssl->own_cert   = own_cert;
    ssl->rsa_key    = rsa_key;
}