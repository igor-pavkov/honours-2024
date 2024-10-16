static int ssl_rsa_sign( void *ctx,
                    int (*f_rng)(void *, unsigned char *, size_t), void *p_rng,
                    int mode, int hash_id, unsigned int hashlen,
                    const unsigned char *hash, unsigned char *sig )
{
    return rsa_pkcs1_sign( (rsa_context *) ctx, f_rng, p_rng, mode, hash_id,
                           hashlen, hash, sig );
}