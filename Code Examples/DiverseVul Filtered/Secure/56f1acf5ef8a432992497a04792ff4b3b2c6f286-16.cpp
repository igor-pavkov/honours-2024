int SSL_set1_param(SSL *ssl, X509_VERIFY_PARAM *vpm)
{
    return X509_VERIFY_PARAM_set1(ssl->param, vpm);
}