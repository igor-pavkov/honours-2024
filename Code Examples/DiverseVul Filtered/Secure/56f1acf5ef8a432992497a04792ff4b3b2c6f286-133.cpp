X509 *ssl_get_server_send_cert(const SSL *s)
{
    CERT_PKEY *cpk;
    cpk = ssl_get_server_send_pkey(s);
    if (!cpk)
        return NULL;
    return cpk->x509;
}