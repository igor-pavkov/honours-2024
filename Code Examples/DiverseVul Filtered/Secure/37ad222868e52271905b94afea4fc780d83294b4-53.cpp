static int doinitsupgroups(const char *user, const uid_t uid, const gid_t gid)
{
#ifndef NON_ROOT_FTP
# ifdef HAVE_SETGROUPS
    if (setgroups(1U, &gid) != 0) {
        return -1;
    }
# else
    (void) gid;
# endif
# ifdef HAVE_INITGROUPS
    if (user == NULL) {
        const struct passwd * const lpwd = getpwuid(uid);

        if (lpwd != NULL && lpwd->pw_name != NULL) {
            user = lpwd->pw_name;
        } else {
            return 0;
        }
    }
    initgroups(user, gid);
# else
    (void) user;
    (void) uid;
# endif
#else
    (void) user;
    (void) uid;
    (void) gid;
#endif
    return 0;
}