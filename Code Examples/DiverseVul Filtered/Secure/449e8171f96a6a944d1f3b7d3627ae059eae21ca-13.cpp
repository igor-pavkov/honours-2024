static void lo_restore_cred(struct lo_cred *old, bool restore_umask)
{
    int res;

    res = syscall(OURSYS_setresuid, -1, old->euid, -1);
    if (res == -1) {
        fuse_log(FUSE_LOG_ERR, "seteuid(%u): %m\n", old->euid);
        exit(1);
    }

    res = syscall(OURSYS_setresgid, -1, old->egid, -1);
    if (res == -1) {
        fuse_log(FUSE_LOG_ERR, "setegid(%u): %m\n", old->egid);
        exit(1);
    }

    if (restore_umask)
        umask(old->umask);
}