void tac_copy_addr_info(struct addrinfo *p_dst, const struct addrinfo *p_src) {
    if (p_dst && p_src) {
        p_dst->ai_flags = p_src->ai_flags;
        p_dst->ai_family = p_src->ai_family;
        p_dst->ai_socktype = p_src->ai_socktype;
        p_dst->ai_protocol = p_src->ai_protocol;
        p_dst->ai_addrlen = p_src->ai_addrlen;

        /* ipv6 check */
        if (p_dst->ai_family == AF_INET6) {
          memcpy (p_dst->ai_addr, p_src->ai_addr, sizeof(struct sockaddr_in6));
          memset ((struct sockaddr_in6*)p_dst->ai_addr, 0 , sizeof(struct sockaddr_in6));
          memcpy ((struct sockaddr_in6*)p_dst->ai_addr, (struct sockaddr_in6*)p_src->ai_addr, sizeof(struct sockaddr_in6));
        } else {
           memcpy (p_dst->ai_addr, p_src->ai_addr, sizeof(struct sockaddr)); 
        }

        p_dst->ai_canonname = NULL; /* we do not care it */
        p_dst->ai_next = NULL;      /* no more chain */
    }
}