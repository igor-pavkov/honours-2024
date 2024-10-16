static int publickey_canauth_cb (LIBSSH2_SESSION *session, unsigned char **sig,
    size_t *sig_len, const unsigned char *data, size_t data_len, void **abstract) {
    return 0;
}