static int final_alpn(SSL *s, unsigned int context, int sent)
{
    if (!s->server && !sent && s->session->ext.alpn_selected != NULL)
            s->ext.early_data_ok = 0;

    if (!s->server || !SSL_IS_TLS13(s))
        return 1;

    /*
     * Call alpn_select callback if needed.  Has to be done after SNI and
     * cipher negotiation (HTTP/2 restricts permitted ciphers). In TLSv1.3
     * we also have to do this before we decide whether to accept early_data.
     * In TLSv1.3 we've already negotiated our cipher so we do this call now.
     * For < TLSv1.3 we defer it until after cipher negotiation.
     *
     * On failure SSLfatal() already called.
     */
    return tls_handle_alpn(s);
}