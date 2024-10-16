void doeprt(char *p)
{
    char delim;
    int family;

    delim = *p++;
    family = atoi(p);
    while (isdigit((unsigned char) *p)) {
        p++;
    }
    if (*p == delim) {
        p++;
    } else {
        addreply_noformat(501, MSG_SYNTAX_ERROR_IP);
        return;
    }
    if (family == 2 && v6ready) {
        do_ipv6_port(p, delim);
        return;
    }
    if (family != 1) {
        if (v6ready) {
            addreply_noformat(522, MSG_ONLY_IPV4V6);
        } else {
            addreply_noformat(522, MSG_ONLY_IPV4);
        }
        return;
    }

    {
        unsigned int a1, a2, a3, a4, port = 0U;
        /* there should be dot-decimal ip as rfc2428 states,
         * but troll used for some reason "comma-decimal" notation
         * so I decided to leave it */
        if ((sscanf(p, "%u,%u,%u,%u", &a1, &a2, &a3, &a4) != 4 &&
             sscanf(p, "%u.%u.%u.%u", &a1, &a2, &a3, &a4) != 4) ||
            a1 > 255U || a2 > 255U || a3 > 255U || a4 > 255U ||
            (a1 | a2 | a3 | a4) == 0U) {
            addreply_noformat(501, MSG_SYNTAX_ERROR_IP);
            return;
        }
        while (*p && strchr("0123456789.,", *p)) {
            p++;
        }
        if (*p == delim) {
            port = (unsigned int) atoi(++p);
            while (*p && isdigit((unsigned char) *p)) {
                p++;
            }
        }
        if (*p != delim || port > 65535U || port <= 0U) {
            addreply_noformat(501, MSG_SYNTAX_ERROR_IP);
            return;
        } else {
            struct sockaddr_storage a;

            memset(&a, 0, sizeof a);
            STORAGE_FAMILY(a) = AF_INET;
            STORAGE_SIN_ADDR(a) =
                htonl(((uint32_t) a1 << 24) |
                      ((uint32_t) a2 << 16) | (a3 << 8) | a4);
            SET_STORAGE_LEN(a, sizeof(struct sockaddr_in));
            doport2(a, port);
        }
    }
}