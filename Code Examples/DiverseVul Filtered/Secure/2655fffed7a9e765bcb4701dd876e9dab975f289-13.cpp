void tcp_cleanup(Slirp *slirp)
{
    while (slirp->tcb.so_next != &slirp->tcb) {
        tcp_close(sototcpcb(slirp->tcb.so_next));
    }
}