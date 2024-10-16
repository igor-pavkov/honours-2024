static unsigned long ssl_session_hash(const SSL_SESSION *a)
{
    unsigned long l;

    l = (unsigned long)
        ((unsigned int)a->session_id[0]) |
        ((unsigned int)a->session_id[1] << 8L) |
        ((unsigned long)a->session_id[2] << 16L) |
        ((unsigned long)a->session_id[3] << 24L);
    return (l);
}