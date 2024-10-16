static int init_session_ticket(SSL *s, unsigned int context)
{
    if (!s->server)
        s->ext.ticket_expected = 0;

    return 1;
}