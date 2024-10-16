static handler_t connection_handle_fdevent(void * const context, const int revents) {
    connection * restrict con = context;
    const int is_ssl_sock = con->is_ssl_sock;

    joblist_append(con);

    if (revents & ~(FDEVENT_IN | FDEVENT_OUT))
        con->revents_err |= (revents & ~(FDEVENT_IN | FDEVENT_OUT));

    if (revents & (FDEVENT_IN | FDEVENT_OUT)) {
        if (is_ssl_sock) /*(ssl may read and write for both reads and writes)*/
            con->is_readable = con->is_writable = 1;
        else {
            if (revents & FDEVENT_IN)
                con->is_readable = 1;
            if (revents & FDEVENT_OUT)
                con->is_writable = 1;
        }
    }

    return HANDLER_FINISHED;
}