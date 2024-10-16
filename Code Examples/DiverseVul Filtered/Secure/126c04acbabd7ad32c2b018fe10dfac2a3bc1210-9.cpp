void ip_init(Slirp *slirp)
{
    slirp->ipq.ip_link.next = slirp->ipq.ip_link.prev = &slirp->ipq.ip_link;
    udp_init(slirp);
    tcp_init(slirp);
    icmp_init(slirp);
}