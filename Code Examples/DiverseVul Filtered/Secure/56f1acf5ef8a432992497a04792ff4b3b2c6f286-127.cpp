long SSL_CTX_ctrl(SSL_CTX *ctx, int cmd, long larg, void *parg)
{
    long l;

    switch (cmd) {
    case SSL_CTRL_GET_READ_AHEAD:
        return (ctx->read_ahead);
    case SSL_CTRL_SET_READ_AHEAD:
        l = ctx->read_ahead;
        ctx->read_ahead = larg;
        return (l);

    case SSL_CTRL_SET_MSG_CALLBACK_ARG:
        ctx->msg_callback_arg = parg;
        return 1;

    case SSL_CTRL_GET_MAX_CERT_LIST:
        return (ctx->max_cert_list);
    case SSL_CTRL_SET_MAX_CERT_LIST:
        l = ctx->max_cert_list;
        ctx->max_cert_list = larg;
        return (l);

    case SSL_CTRL_SET_SESS_CACHE_SIZE:
        l = ctx->session_cache_size;
        ctx->session_cache_size = larg;
        return (l);
    case SSL_CTRL_GET_SESS_CACHE_SIZE:
        return (ctx->session_cache_size);
    case SSL_CTRL_SET_SESS_CACHE_MODE:
        l = ctx->session_cache_mode;
        ctx->session_cache_mode = larg;
        return (l);
    case SSL_CTRL_GET_SESS_CACHE_MODE:
        return (ctx->session_cache_mode);

    case SSL_CTRL_SESS_NUMBER:
        return (lh_SSL_SESSION_num_items(ctx->sessions));
    case SSL_CTRL_SESS_CONNECT:
        return (ctx->stats.sess_connect);
    case SSL_CTRL_SESS_CONNECT_GOOD:
        return (ctx->stats.sess_connect_good);
    case SSL_CTRL_SESS_CONNECT_RENEGOTIATE:
        return (ctx->stats.sess_connect_renegotiate);
    case SSL_CTRL_SESS_ACCEPT:
        return (ctx->stats.sess_accept);
    case SSL_CTRL_SESS_ACCEPT_GOOD:
        return (ctx->stats.sess_accept_good);
    case SSL_CTRL_SESS_ACCEPT_RENEGOTIATE:
        return (ctx->stats.sess_accept_renegotiate);
    case SSL_CTRL_SESS_HIT:
        return (ctx->stats.sess_hit);
    case SSL_CTRL_SESS_CB_HIT:
        return (ctx->stats.sess_cb_hit);
    case SSL_CTRL_SESS_MISSES:
        return (ctx->stats.sess_miss);
    case SSL_CTRL_SESS_TIMEOUTS:
        return (ctx->stats.sess_timeout);
    case SSL_CTRL_SESS_CACHE_FULL:
        return (ctx->stats.sess_cache_full);
    case SSL_CTRL_OPTIONS:
        return (ctx->options |= larg);
    case SSL_CTRL_CLEAR_OPTIONS:
        return (ctx->options &= ~larg);
    case SSL_CTRL_MODE:
        return (ctx->mode |= larg);
    case SSL_CTRL_CLEAR_MODE:
        return (ctx->mode &= ~larg);
    case SSL_CTRL_SET_MAX_SEND_FRAGMENT:
        if (larg < 512 || larg > SSL3_RT_MAX_PLAIN_LENGTH)
            return 0;
        ctx->max_send_fragment = larg;
        return 1;
    default:
        return (ctx->method->ssl_ctx_ctrl(ctx, cmd, larg, parg));
    }
}