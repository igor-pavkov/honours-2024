static int final_sig_algs(SSL *s, unsigned int context, int sent)
{
    if (!sent && SSL_IS_TLS13(s) && !s->hit) {
        SSLfatal(s, TLS13_AD_MISSING_EXTENSION, SSL_F_FINAL_SIG_ALGS,
                 SSL_R_MISSING_SIGALGS_EXTENSION);
        return 0;
    }

    return 1;
}