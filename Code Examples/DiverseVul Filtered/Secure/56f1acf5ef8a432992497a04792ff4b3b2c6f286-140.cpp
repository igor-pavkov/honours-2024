int SSL_set_trust(SSL *s, int trust)
{
    return X509_VERIFY_PARAM_set_trust(s->param, trust);
}