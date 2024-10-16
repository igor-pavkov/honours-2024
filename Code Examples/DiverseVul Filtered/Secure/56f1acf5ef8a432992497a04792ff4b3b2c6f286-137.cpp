void SSL_set_tmp_rsa_callback(SSL *ssl, RSA *(*cb) (SSL *ssl,
                                                    int is_export,
                                                    int keylength))
{
    SSL_callback_ctrl(ssl, SSL_CTRL_SET_TMP_RSA_CB, (void (*)(void))cb);
}