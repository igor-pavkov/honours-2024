int SSL_CTX_set_trust(SSL_CTX *s, int trust)
{
    return X509_VERIFY_PARAM_set_trust(s->param, trust);
}