void SSL_set_tmp_dh_callback(SSL *ssl, DH *(*dh) (SSL *ssl, int is_export,
                                                  int keylength))
{
    SSL_callback_ctrl(ssl, SSL_CTRL_SET_TMP_DH_CB, (void (*)(void))dh);
}