static int doport3(const int protocol)
{
    if ((datafd = privsep_bindresport(protocol, ctrlconn)) == -1) {
        error(425, MSG_CANT_CREATE_DATA_SOCKET);

        return -1;
    }
    return 0;
}