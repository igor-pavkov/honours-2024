uint8_t tcp_tos(struct socket *so)
{
    int i = 0;

    while (tcptos[i].tos) {
        if ((tcptos[i].fport && (ntohs(so->so_fport) == tcptos[i].fport)) ||
            (tcptos[i].lport && (ntohs(so->so_lport) == tcptos[i].lport))) {
            if (so->slirp->enable_emu)
                so->so_emu = tcptos[i].emu;
            return tcptos[i].tos;
        }
        i++;
    }
    return 0;
}