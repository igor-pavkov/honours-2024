void tcp_attach(struct socket *so)
{
    so->so_tcpcb = tcp_newtcpcb(so);
    insque(so, &so->slirp->tcb);
}