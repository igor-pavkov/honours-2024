void tcp_init(Slirp *slirp)
{
    slirp->tcp_iss = 1; /* wrong */
    slirp->tcb.so_next = slirp->tcb.so_prev = &slirp->tcb;
    slirp->tcp_last_so = &slirp->tcb;
}