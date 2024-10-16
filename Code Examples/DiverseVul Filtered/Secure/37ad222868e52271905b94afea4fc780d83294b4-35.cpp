static void keepalive(const int fd, int keep)
{
#ifdef SO_KEEPALIVE
    {
        setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, (char *) &keep, sizeof keep);
    }
#endif
}