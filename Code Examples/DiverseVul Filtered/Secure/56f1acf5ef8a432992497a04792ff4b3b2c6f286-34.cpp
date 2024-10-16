void SSL_CTX_set_tmp_rsa_callback(SSL_CTX *ctx, RSA *(*cb) (SSL *ssl,
                                                            int is_export,
                                                            int keylength))
{
    SSL_CTX_callback_ctrl(ctx, SSL_CTRL_SET_TMP_RSA_CB, (void (*)(void))cb);
}