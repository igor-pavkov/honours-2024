const SSL_CIPHER *SSL_get_current_cipher(const SSL *s)
{
    if ((s->session != NULL) && (s->session->cipher != NULL))
        return (s->session->cipher);
    return (NULL);
}