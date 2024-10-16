int (*SSL_get_verify_callback(const SSL *s)) (int, X509_STORE_CTX *) {
    return (s->verify_callback);
}