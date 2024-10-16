int ssh_get_random(void *where, int len, int strong)
{
#ifdef HAVE_OPENSSL_RAND_PRIV_BYTES
    if (strong) {
        /* Returns -1 when not supported, 0 on error, 1 on success */
        return !!RAND_priv_bytes(where, len);
    }
#else
    (void)strong;
#endif /* HAVE_RAND_PRIV_BYTES */

    /* Returns -1 when not supported, 0 on error, 1 on success */
    return !!RAND_bytes(where, len);
}