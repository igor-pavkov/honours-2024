struct tcpcb *tcp_drop(struct tcpcb *tp, int err)
{
    DEBUG_CALL("tcp_drop");
    DEBUG_ARG("tp = %p", tp);
    DEBUG_ARG("errno = %d", errno);

    if (TCPS_HAVERCVDSYN(tp->t_state)) {
        tp->t_state = TCPS_CLOSED;
        (void)tcp_output(tp);
    }
    return (tcp_close(tp));
}