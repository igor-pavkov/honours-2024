void ip_cleanup(Slirp *slirp)
{
    udp_cleanup(slirp);
    tcp_cleanup(slirp);
    icmp_cleanup(slirp);
}