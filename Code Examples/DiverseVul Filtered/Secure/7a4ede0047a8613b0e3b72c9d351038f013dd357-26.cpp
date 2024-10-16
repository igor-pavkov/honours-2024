static int oss_get_version (int fd, int *version, const char *typ)
{
    if (ioctl (fd, OSS_GETVERSION, &version)) {
#if defined(__FreeBSD__) || defined(__FreeBSD_kernel__)
        /*
         * Looks like atm (20100109) FreeBSD knows OSS_GETVERSION
         * since 7.x, but currently only on the mixer device (or in
         * the Linuxolator), and in the native version that part of
         * the code is in fact never reached so the ioctl fails anyway.
         * Until this is fixed, just check the errno and if its what
         * FreeBSD's sound drivers return atm assume they are new enough.
         */
        if (errno == EINVAL) {
            *version = 0x040000;
            return 0;
        }
#endif
        oss_logerr2 (errno, typ, "Failed to get OSS version\n");
        return -1;
    }
    return 0;
}