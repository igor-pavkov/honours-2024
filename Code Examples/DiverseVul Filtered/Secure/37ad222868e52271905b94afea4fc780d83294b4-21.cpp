void dochmod(char *name, mode_t mode)
{
    static dev_t root_st_dev;
    static ino_t root_st_ino;
    struct stat st2;
    int fd = -1;

    if (nochmod != 0 && authresult.uid != (uid_t) 0) {
        addreply(550, MSG_CHMOD_FAILED, name);
        return;
    }
# ifndef ANON_CAN_CHANGE_PERMS
    if (guest != 0) {
        addreply_noformat(550, MSG_ANON_CANT_CHANGE_PERMS);
        return;
    }
# endif
    if (name == NULL || *name == 0) {
        addreply_noformat(501, MSG_NO_FILE_NAME);
        return;
    }
    if (checknamesanity(name, dot_write_ok) != 0) {
        addreply(553, MSG_SANITY_FILE_FAILURE, name);
        return;
    }
    fd = open(name, O_RDONLY);
    if (fd == -1) {
        goto failure;
    }
    if ((root_st_dev | root_st_ino) == 0) {
        struct stat st;

        if (stat("/", &st) != 0) {
            goto failure;
        }
        root_st_dev = st.st_dev;
        root_st_ino = st.st_ino;
    }
    if (fstat(fd, &st2) != 0) {
        goto failure;
    }
# ifdef QUOTAS
    if (hasquota() == 0 && S_ISDIR(st2.st_mode)) {
        mode |= 0500;
    }
# endif
    if (st2.st_ino == root_st_ino && st2.st_dev == root_st_dev) {
        mode |= 0700;
    } else if (be_customer_proof != 0) {
        mode |= (S_ISDIR(st2.st_mode) ? 0700 : 0600);
    }
    if (fchmod(fd, mode) < 0 && chmod(name, mode) < 0) {
        failure:
        if (fd != -1) {
            (void) close(fd);
        }
        addreply(550, MSG_CHMOD_FAILED ": %s", name, strerror(errno));
        return;
    }
    (void) close(fd);
    addreply(200, MSG_CHMOD_SUCCESS, name);
}