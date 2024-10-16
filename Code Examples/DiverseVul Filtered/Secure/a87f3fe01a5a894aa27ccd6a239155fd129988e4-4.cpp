int tls1_lookup_md(SSL_CTX *ctx, const SIGALG_LOOKUP *lu, const EVP_MD **pmd)
{
    const EVP_MD *md;
    if (lu == NULL)
        return 0;
    /* lu->hash == NID_undef means no associated digest */
    if (lu->hash == NID_undef) {
        md = NULL;
    } else {
        md = ssl_md(ctx, lu->hash_idx);
        if (md == NULL)
            return 0;
    }
    if (pmd)
        *pmd = md;
    return 1;
}