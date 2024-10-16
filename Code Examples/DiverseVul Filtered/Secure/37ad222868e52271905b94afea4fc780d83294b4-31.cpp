static int ul_send(ULHandler * const ulhandler)
{
    double ts_start = 0.0;
    off_t uploaded = (off_t) 0;
    int pollret;
    int timeout;
    int ret;

    if (ulhandler->bandwidth > 0UL && (ts_start = get_usec_time()) <= 0.0) {
        error(451, "gettimeofday()");
        return -1;
    }
    for (;;) {
        if (ulhandler->idletime >= INT_MAX / 1000) {
            timeout = INT_MAX;
        } else {
            timeout = (int) ulhandler->idletime * 1000;
        }
        ulhandler->pfds[PFD_DATA].revents =
            ulhandler->pfds[PFD_COMMANDS].revents = 0;
        pollret = poll(ulhandler->pfds,
                       sizeof ulhandler->pfds / sizeof ulhandler->pfds[0],
                       timeout);
        if (pollret < 0) {
            addreply_noformat(451, MSG_DATA_READ_FAILED);
            return -1;
        }
        if (pollret == 0) {
            addreply_noformat(421, MSG_TIMEOUT);
            return -1;
        }
        if ((ulhandler->pfds[PFD_DATA].revents & POLLIN) != 0) {
            ret = ul_handle_data(ulhandler, &uploaded, ts_start);
            switch (ret) {
            case 1:
                return 1;
            case 2:
                return 0;
            case 0:
                break;
            default:
                if (ret > 2) {
                    abort();
                }
                return ret;
            }
        }
        if ((ulhandler->pfds[PFD_COMMANDS].revents & (POLLIN | POLLPRI)) != 0) {
            ret = ulhandler_handle_commands(ulhandler);
            if (ret != 0) {
                return ret;
            }
        }
        if ((ulhandler->pfds[PFD_DATA].revents & (POLLERR | POLLNVAL)) != 0 ||
            ((ulhandler->pfds[PFD_DATA].revents & POLLHUP) != 0 &&
             (ulhandler->pfds[PFD_DATA].revents & POLLIN) == 0)) {
            return -1;
        }
        if ((ulhandler->pfds[PFD_COMMANDS].revents &
             (POLLERR | POLLHUP | POLLNVAL)) != 0) {
            addreply_noformat(221, MSG_LOGOUT);
            return -1;
        }
    }
    /* NOTREACHED */
    return 0;
}