void doport(const char *arg)
{
    unsigned int a1, a2, a3, a4, p1, p2;
    struct sockaddr_storage a;

    if (sscanf(arg, "%u,%u,%u,%u,%u,%u",
               &a1, &a2, &a3, &a4, &p1, &p2) != 6 ||
        a1 > 255 || a2 > 255 || a3 > 255 || a4 > 255 ||
        p1 > 255 || p2 > 255 || (a1|a2|a3|a4) == 0 ||
        (p1 | p2) == 0) {
        addreply_noformat(501, MSG_SYNTAX_ERROR_IP);
        return;
    }
    memset(&a, 0, sizeof a);
    STORAGE_FAMILY(a) = AF_INET;
    STORAGE_SIN_ADDR(a) =
        htonl(((uint32_t) a1 << 24) | ((uint32_t) a2 << 16) |
              (a3 << 8) | a4);
    SET_STORAGE_LEN(a, sizeof(struct sockaddr_in));
    doport2(a, (p1 << 8) | p2);
}