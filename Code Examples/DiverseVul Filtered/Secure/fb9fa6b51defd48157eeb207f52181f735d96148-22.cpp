static int init_certificate_authorities(SSL *s, unsigned int context)
{
    sk_X509_NAME_pop_free(s->s3->tmp.peer_ca_names, X509_NAME_free);
    s->s3->tmp.peer_ca_names = NULL;
    return 1;
}