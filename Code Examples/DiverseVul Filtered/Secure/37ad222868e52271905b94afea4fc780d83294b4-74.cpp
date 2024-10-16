static int addrcmp(const struct sockaddr_storage * const s1,
                   const struct sockaddr_storage * const s2)
{
    if (STORAGE_FAMILY(*s1) == AF_INET6) {
        if (STORAGE_FAMILY(*s2) != AF_INET6) {
            return 1;
        }
        if (IN6_ARE_ADDR_EQUAL(&STORAGE_SIN_ADDR6_NF_CONST(*s1), &STORAGE_SIN_ADDR6_NF_CONST(*s2))) {
            return 0;
        } else {
            return 1;
        }
    } else if (STORAGE_FAMILY(*s1) == AF_INET) {
        if (STORAGE_FAMILY(*s2) != AF_INET) {
            return 1;
        }
        if (STORAGE_SIN_ADDR_CONST(*s1) == STORAGE_SIN_ADDR_CONST(*s2)) {
            return 0;
        } else {
            return 1;
        }
    }
    return -1;
}