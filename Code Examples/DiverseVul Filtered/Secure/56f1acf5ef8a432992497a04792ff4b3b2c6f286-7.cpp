void SSL_set_tmp_ecdh_callback(SSL *ssl,
                               EC_KEY *(*ecdh) (SSL *ssl, int is_export,
                                                int keylength))
{
    SSL_callback_ctrl(ssl, SSL_CTRL_SET_TMP_ECDH_CB, (void (*)(void))ecdh);
}