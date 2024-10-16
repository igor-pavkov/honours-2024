static int generic_aton(const char *src, struct sockaddr_storage *a)
{
    if (inet_pton(AF_INET6, src, &STORAGE_SIN_ADDR6(*a)) > 0) {
        STORAGE_FAMILY(*a) = AF_INET6;
        return 0;
    }
    if (inet_pton(AF_INET, src, &STORAGE_SIN_ADDR(*a)) > 0) {
        STORAGE_FAMILY(*a) = AF_INET;
        return 0;
    }
    memset(a, 0, sizeof *a);

    return -1;
}