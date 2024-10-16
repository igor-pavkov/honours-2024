static int ssl_check_srp_ext_ClientHello(SSL *s, int *al)
{
    int ret = SSL_ERROR_NONE;

    *al = SSL_AD_UNRECOGNIZED_NAME;

    if ((s->s3->tmp.new_cipher->algorithm_mkey & SSL_kSRP) &&
        (s->srp_ctx.TLS_ext_srp_username_callback != NULL)) {
        if (s->srp_ctx.login == NULL) {
            /*
             * RFC 5054 says SHOULD reject, we do so if There is no srp
             * login name
             */
            ret = SSL3_AL_FATAL;
            *al = SSL_AD_UNKNOWN_PSK_IDENTITY;
        } else {
            ret = SSL_srp_server_param_with_username(s, al);
        }
    }
    return ret;
}