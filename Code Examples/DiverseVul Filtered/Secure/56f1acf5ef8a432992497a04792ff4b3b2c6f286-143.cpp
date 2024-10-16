char *SSL_get_shared_ciphers(const SSL *s, char *buf, int len)
{
    char *p;
    STACK_OF(SSL_CIPHER) *sk;
    SSL_CIPHER *c;
    int i;

    if ((s->session == NULL) || (s->session->ciphers == NULL) || (len < 2))
        return (NULL);

    p = buf;
    sk = s->session->ciphers;

    if (sk_SSL_CIPHER_num(sk) == 0)
        return NULL;

    for (i = 0; i < sk_SSL_CIPHER_num(sk); i++) {
        int n;

        c = sk_SSL_CIPHER_value(sk, i);
        n = strlen(c->name);
        if (n + 1 > len) {
            if (p != buf)
                --p;
            *p = '\0';
            return buf;
        }
        strcpy(p, c->name);
        p += n;
        *(p++) = ':';
        len -= n + 1;
    }
    p[-1] = '\0';
    return (buf);
}