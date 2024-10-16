static int checkvalidaddr(const struct sockaddr_storage * const addr)
{
    if (addr == NULL) {
        return 0;
    }
    /* Some versions of MacOS X have broken IN* macros */
#ifdef __APPLE_CC__
    return 1;
#endif
    if (STORAGE_FAMILY(*addr) == AF_INET6) {
        if (IN6_IS_ADDR_MULTICAST(&STORAGE_SIN_ADDR6_NF_CONST(*addr)) ||
            IN6_IS_ADDR_UNSPECIFIED(&STORAGE_SIN_ADDR6_NF_CONST(*addr))) {
            return 0;
        }
        return 1;
    } else if (STORAGE_FAMILY(*addr) == AF_INET) {
        if (ntohl(STORAGE_SIN_ADDR_CONST(*addr)) == INADDR_ANY ||
            ntohl(STORAGE_SIN_ADDR_CONST(*addr)) == INADDR_NONE ||
            ntohl(STORAGE_SIN_ADDR_CONST(*addr)) == INADDR_BROADCAST ||
            IN_MULTICAST(ntohl(STORAGE_SIN_ADDR_CONST(*addr)))) {
            return 0;
        }
        return 1;
    }
    return 0;
}