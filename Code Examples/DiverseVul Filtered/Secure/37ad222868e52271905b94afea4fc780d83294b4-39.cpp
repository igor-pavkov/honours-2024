static int checknamesanity(const char *name, int dot_ok)
{
    const char *namepnt;

#ifdef PARANOID_FILE_NAMES
    const char *validchars =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefgihjklmnopqrstuvwxyz"
        "0123456789./-_";
#endif

    if (name == NULL || *name == 0) {
        return -1;
    }
    /* optimize . and .. */
    if (name[0] == '.' && (name[1] == 0 || (name[1] == '.' && name[2] == 0))) {
        return 0;
    }
    namepnt = name;
#ifdef PARANOID_FILE_NAMES
    /* we want to make sure we don't get any non-alphanumeric file name */
    if (strlen(namepnt) != strspn(namepnt, validchars)) {
        return -1;
    }
#endif
#ifdef QUOTAS
    if (hasquota() == 0) {
        if (strstr(namepnt, QUOTA_FILE) != NULL) {
            return -1;                     /* .ftpquota => *NO* */
        }
# ifndef ALLOW_DELETION_OF_TEMPORARY_FILES
        if (strstr(namepnt, PUREFTPD_TMPFILE_PREFIX) == namepnt) {
            return -1;
        }
# endif
    }
#endif
    while (*namepnt != 0) {
#ifndef ALLOW_EVERYTHING_IN_FILE_NAMES
        if (ISCTRLCODE(*namepnt) || *namepnt == '\\') {
            return -1;
        }
#endif
        if (dot_ok == 0) {
            if (*namepnt == '/') {
                namepnt++;
            } else if (namepnt != name) {
                namepnt++;
                continue;
            }
            if (namepnt[0] == 0) {     /* /$ */
                return 0;
            }
            if (namepnt[0] == '.') {   /* /. */
                if (namepnt[1] == 0) { /* /.$ => ok */
                    return 0;
                }
                if (namepnt[1] == '.') {   /* /.. */
                    if (namepnt[2] == 0) {   /* /..$ => ok */
                        return 0;
                    }
                    if (namepnt[2] != '/') {   /* /..[^/] => *NO* */
                        return -1;
                    }
                } else if (namepnt[1] != '/') {   /* /.[^/]/ => *NO* */
                    return -1;
                }
            }
            if (namepnt != name) {
                continue;
            }
        }
        namepnt++;
    }
    return 0;
}