static int check_trustedgroup(const uid_t uid, const gid_t gid)
{
    GETGROUPS_T *alloca_suppgroups;
    int n;
    int n2;
    int result = 0;

    if (uid == (uid_t) 0) {
        return 1;
    }
    if (userchroot == 2) {
        return 0;
    }
    if (gid == chroot_trustedgid) {
        return 1;
    }
#ifdef HAVE_GETGROUPS
    if ((n = getgroups(0, NULL)) <= 0) {
        return 0;
    }
    if ((alloca_suppgroups =
         ALLOCA(n * (sizeof *alloca_suppgroups))) == NULL) {
        die_mem();
    }
    n2 = getgroups(n, alloca_suppgroups);
    /* Jedi's paranoia */
    if (n2 < n) {
        n = n2;
    }
    result = 0;
    while (n != 0) {
        n--;
        if (alloca_suppgroups[n] == (GETGROUPS_T) chroot_trustedgid) {
            result = 1;
            break;
        }
    };
    ALLOCA_FREE(alloca_suppgroups);
#endif

    return result;
}