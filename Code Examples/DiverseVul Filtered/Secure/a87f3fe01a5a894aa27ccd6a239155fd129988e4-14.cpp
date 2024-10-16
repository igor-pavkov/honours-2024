static int sigalg_security_bits(SSL_CTX *ctx, const SIGALG_LOOKUP *lu)
{
    const EVP_MD *md = NULL;
    int secbits = 0;

    if (!tls1_lookup_md(ctx, lu, &md))
        return 0;
    if (md != NULL)
    {
        /* Security bits: half digest bits */
        secbits = EVP_MD_size(md) * 4;
    } else {
        /* Values from https://tools.ietf.org/html/rfc8032#section-8.5 */
        if (lu->sigalg == TLSEXT_SIGALG_ed25519)
            secbits = 128;
        else if (lu->sigalg == TLSEXT_SIGALG_ed448)
            secbits = 224;
    }
    return secbits;
}