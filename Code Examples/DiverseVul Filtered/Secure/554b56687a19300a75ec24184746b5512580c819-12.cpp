static void conn_to_str(const conn *c, char *addr, char *svr_addr) {
    if (!c) {
        strcpy(addr, "<null>");
    } else if (c->state == conn_closed) {
        strcpy(addr, "<closed>");
    } else {
        struct sockaddr_in6 local_addr;
        struct sockaddr *sock_addr = (void *)&c->request_addr;

        /* For listen ports and idle UDP ports, show listen address */
        if (c->state == conn_listening ||
                (IS_UDP(c->transport) &&
                 c->state == conn_read)) {
            socklen_t local_addr_len = sizeof(local_addr);

            if (getsockname(c->sfd,
                        (struct sockaddr *)&local_addr,
                        &local_addr_len) == 0) {
                sock_addr = (struct sockaddr *)&local_addr;
            }
        }
        get_conn_text(c, sock_addr->sa_family, addr, sock_addr);

        if (c->state != conn_listening && !(IS_UDP(c->transport) &&
                 c->state == conn_read)) {
            struct sockaddr_storage svr_sock_addr;
            socklen_t svr_addr_len = sizeof(svr_sock_addr);
            getsockname(c->sfd, (struct sockaddr *)&svr_sock_addr, &svr_addr_len);
            get_conn_text(c, svr_sock_addr.ss_family, svr_addr, (struct sockaddr *)&svr_sock_addr);
        }
    }
}