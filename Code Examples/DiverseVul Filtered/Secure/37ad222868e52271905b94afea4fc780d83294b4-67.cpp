static int check_standalone(void)
{
    socklen_t socksize = (socklen_t) sizeof ctrlconn;
    if (getsockname(0, (struct sockaddr *) &ctrlconn, &socksize) != 0) {
        clientfd = -1;
        return 1;
    }
    if (dup2(0, 1) == -1) {
        _EXIT(EXIT_FAILURE);
    }
    clientfd = 0;

    return 0;
}