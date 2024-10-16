static int final_maxfragmentlen(SSL *s, unsigned int context, int sent)
{
    /*
     * Session resumption on server-side with MFL extension active
     *  BUT MFL extension packet was not resent (i.e. sent == 0)
     */
    if (s->server && s->hit && USE_MAX_FRAGMENT_LENGTH_EXT(s->session)
            && !sent ) {
        SSLfatal(s, SSL_AD_MISSING_EXTENSION, SSL_F_FINAL_MAXFRAGMENTLEN,
                 SSL_R_BAD_EXTENSION);
        return 0;
    }

    /* Current SSL buffer is lower than requested MFL */
    if (s->session && USE_MAX_FRAGMENT_LENGTH_EXT(s->session)
            && s->max_send_fragment < GET_MAX_FRAGMENT_LENGTH(s->session))
        /* trigger a larger buffer reallocation */
        if (!ssl3_setup_buffers(s)) {
            /* SSLfatal() already called */
            return 0;
        }

    return 1;
}