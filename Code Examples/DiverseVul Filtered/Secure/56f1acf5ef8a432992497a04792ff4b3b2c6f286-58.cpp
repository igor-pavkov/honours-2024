long SSL_ctrl(SSL *s, int cmd, long larg, void *parg)
{
    long l;

    switch (cmd) {
    case SSL_CTRL_GET_READ_AHEAD:
        return (s->read_ahead);
    case SSL_CTRL_SET_READ_AHEAD:
        l = s->read_ahead;
        s->read_ahead = larg;
        return (l);

    case SSL_CTRL_SET_MSG_CALLBACK_ARG:
        s->msg_callback_arg = parg;
        return 1;

    case SSL_CTRL_OPTIONS:
        return (s->options |= larg);
    case SSL_CTRL_CLEAR_OPTIONS:
        return (s->options &= ~larg);
    case SSL_CTRL_MODE:
        return (s->mode |= larg);
    case SSL_CTRL_CLEAR_MODE:
        return (s->mode &= ~larg);
    case SSL_CTRL_GET_MAX_CERT_LIST:
        return (s->max_cert_list);
    case SSL_CTRL_SET_MAX_CERT_LIST:
        l = s->max_cert_list;
        s->max_cert_list = larg;
        return (l);
    case SSL_CTRL_SET_MAX_SEND_FRAGMENT:
        if (larg < 512 || larg > SSL3_RT_MAX_PLAIN_LENGTH)
            return 0;
        s->max_send_fragment = larg;
        return 1;
    case SSL_CTRL_GET_RI_SUPPORT:
        if (s->s3)
            return s->s3->send_connection_binding;
        else
            return 0;
    default:
        return (s->method->ssl_ctrl(s, cmd, larg, parg));
    }
}