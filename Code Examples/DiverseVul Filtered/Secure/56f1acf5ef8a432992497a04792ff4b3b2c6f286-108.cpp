int SSL_get_verify_depth(const SSL *s)
{
    return X509_VERIFY_PARAM_get_depth(s->param);
}