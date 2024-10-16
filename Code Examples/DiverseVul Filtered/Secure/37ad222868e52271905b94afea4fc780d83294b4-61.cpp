static int ul_handle_data(ULHandler * const ulhandler, off_t * const uploaded,
                          const double ts_start)
{
    ssize_t readnb;
    double required_sleep = 0.0;
    int pollret;
    int ret;

    if (ulhandler->max_filesize >= (off_t) 0 &&
        ulhandler->total_uploaded > ulhandler->max_filesize) {
        addreply(552, MSG_ABORTED " (quota)");
        return -2;
    }
    if (ulhandler->chunk_size > (off_t) ulhandler->sizeof_buf) {
        ulhandler->chunk_size = ulhandler->max_chunk_size =
            ulhandler->sizeof_buf;
    }
    if (ulhandler->tls_fd != NULL) {
#ifdef WITH_TLS
        readnb = SSL_read(ulhandler->tls_fd, ulhandler->buf,
                          ulhandler->chunk_size);
#else
        abort();
#endif
    } else {
        readnb = read(ulhandler->xferfd, ulhandler->buf,
                      ulhandler->chunk_size);
    }
    if (readnb == (ssize_t) 0) {
        return 2;
    }
    if (readnb < (ssize_t) 0) {
        if (errno == EAGAIN || errno == EINTR) {
            return 0;
        }
        addreply_noformat(451, MSG_DATA_READ_FAILED);
        return -1;
    }
    if (ul_dowrite(ulhandler, ulhandler->buf, readnb, uploaded) != 0) {
        addreply_noformat(452, MSG_WRITE_FAILED);
        return -1;
    }
    ulhandler->cur_pos += *uploaded;
#ifdef FTPWHO
        if (shm_data_cur != NULL) {
            shm_data_cur->download_current_size =
                shm_data_cur->download_total_size = ulhandler->cur_pos;
        }
#endif
    ulhandler->total_uploaded += *uploaded;
    if (ulhandler->bandwidth > 0UL) {
        ulhandler_throttle(ulhandler, *uploaded, ts_start, &required_sleep);
        if (required_sleep > 0.0) {
            repoll:
            ulhandler->pfds_command.revents = 0;
            pollret = poll(&ulhandler->pfds_command, 1, required_sleep * 1000.0);
            if (pollret == 0) {
                return 0;
            }
            if (pollret < 0) {
                if (errno == EINTR) {
                    goto repoll;
                }
                return -1;
            }
            if ((ulhandler->pfds_command.revents &
                 (POLLERR | POLLHUP | POLLNVAL)) != 0) {
                return -1;
            }
            if ((ulhandler->pfds_command.revents & (POLLIN | POLLPRI)) != 0) {
                ret = ulhandler_handle_commands(ulhandler);
                if (ret != 0) {
                    return ret;
                }
                goto repoll;
            }
        }
    }
    return 0;
}