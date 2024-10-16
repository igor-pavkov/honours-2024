static void check_ipv6_support(void)     /* check for ipv6 support in kernel */
{
#ifndef OLD_IP_STACK
    int p;

    if ((p = socket(PF_INET6, SOCK_STREAM, IPPROTO_TCP)) != -1) {
        (void) close(p);
        v6ready++;
    }
#endif
}