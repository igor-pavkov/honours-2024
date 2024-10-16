int modernformat(const char *file, char *target, size_t target_size,
                 const char * const prefix)
{
    char link_target[PATH_MAX + 1U];
    const char *ft;
    const char *ftx = "";
    struct tm *t;
    struct stat st;
    int ret = 0;

    if (lstat(file, &st) != 0 || !(t = gmtime((time_t *) &st.st_mtime))) {
        return -1;
    }
#if !defined(MINIMAL) && !defined(ALWAYS_SHOW_SYMLINKS_AS_SYMLINKS)
    if (
# ifndef ALWAYS_SHOW_RESOLVED_SYMLINKS
        broken_client_compat != 0 &&
# endif
        S_ISLNK(st.st_mode)) {
        struct stat sts;

        if (stat(file, &sts) == 0 && !S_ISLNK(sts.st_mode)) {
            st = sts;
        }
    } /* Show non-dangling symlinks as files/directories */
#endif
    if (S_ISREG(st.st_mode)) {
        ft = "file";
    } else if (S_ISDIR(st.st_mode)) {
        ret = 1;
        ft = "dir";
        if (*file == '.') {
            if (file[1] == '.' && file[2] == 0) {
                ft = "pdir";
            } else if (file[1] == 0) {
                ft = "cdir";
            }
        } else if (*file == '/' && file[1] == 0) {
            ft = "pdir";
        }
    } else if (S_ISLNK(st.st_mode)) {
        ssize_t sx;

        ft = "OS.unix=symlink";
        if ((sx = readlink(file, link_target, sizeof link_target - 1U)) > 0) {
            link_target[sx] = 0;
            if (strpbrk(link_target, "\r\n;") == NULL) {
                ftx = link_target;
                ft = "OS.unix=slink:";
            }
        }
    } else {
        ft = "unknown";
    }
    if (guest != 0) {
        if (SNCHECK(snprintf(target, target_size,
                             "%stype=%s%s;siz%c=%llu;modify=%04d%02d%02d%02d%02d%02d;UNIX.mode=0%o;unique=%xg%llx; %s",
                             prefix,
                             ft, ftx,
                             ret ? 'd' : 'e',
                             (unsigned long long) st.st_size,
                             t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
                             t->tm_hour, t->tm_min, t->tm_sec,
                             (unsigned int) st.st_mode & 07777,
                             (unsigned int) st.st_dev,
                             (unsigned long long) st.st_ino,
                             file), target_size)) {
            _EXIT(EXIT_FAILURE);
        }
    } else {
        if (SNCHECK(snprintf(target, target_size,
                             "%stype=%s;siz%c=%llu;modify=%04d%02d%02d%02d%02d%02d;UNIX.mode=0%o;UNIX.uid=%lld;UNIX.gid=%lld;unique=%xg%llx; %s",
                             prefix,
                             ft,
                             ret ? 'd' : 'e',
                             (unsigned long long) st.st_size,
                             t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
                             t->tm_hour, t->tm_min, t->tm_sec,
                             (unsigned int) st.st_mode & 07777,
                             (unsigned long long) st.st_uid,
                             (unsigned long long) st.st_gid,
                             (unsigned int) st.st_dev,
                             (unsigned long long) st.st_ino,
                             file), target_size)) {
            _EXIT(EXIT_FAILURE);
        }
    }
    return ret;
}