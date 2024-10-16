int SSL_set_purpose(SSL *s, int purpose)
{
    return X509_VERIFY_PARAM_set_purpose(s->param, purpose);
}