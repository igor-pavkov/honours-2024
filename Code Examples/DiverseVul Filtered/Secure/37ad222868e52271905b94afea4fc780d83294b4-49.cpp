void docwd(const char *dir)
{
    const char *where;
    char buffer[PATH_MAX + 256U];

    if (loggedin == 0) {
        goto kaboom;
    }
    /*
     * secure and conformant tilde expansion routine. Need to be packaged in
     * a function so that it can be called in other commands and avoid
     * duplicate code in ls.c             -frank.
     */
    where = dir;
    if (dir == NULL || *dir == 0) {
        dir = "~";
    }
    if (*dir == '~') {
        const struct passwd *pw;

        if (dir[1] == 0) {         /* cd ~ */
            strncpy(buffer, chrooted != 0 ? "/" : authresult.dir,
                    sizeof buffer);
            buffer[sizeof buffer - (size_t) 1U] = 0;
            where = buffer;
        } else {                   /* cd ~user or cd ~user/ */
            char *bufpnt = buffer;
            size_t s = sizeof buffer;
            const char *dirscan = dir + 1;

            while (*dirscan != 0 && *dirscan != '/') {
                if (--s <= 0) {
                    goto kaboom;   /* script kiddy's playing */
                }
                *bufpnt++ = *dirscan++;
            }
            *bufpnt = 0;
            if (*buffer == 0) {        /* ~/... */
                snprintf(buffer, sizeof buffer, "%s%s",
                         chrooted != 0 ? "/" : authresult.dir, dirscan);
                where = buffer;
            } else if (authresult.slow_tilde_expansion == 0) {
                if (chrooted != 0 || guest != 0 ||
                    (pw = getpwnam(buffer)) == NULL || pw->pw_dir == NULL) {
                    /* try with old where = dir */
                } else {
                    snprintf(buffer, sizeof buffer, "%s%s", pw->pw_dir, dirscan);
                    where = buffer;
                }
            }
        }
    }
    if (checknamesanity(where, dot_read_ok) != 0) {
        addreply(553, MSG_SANITY_FILE_FAILURE, where);
        return;
    }
    if (chdir(where) != 0) {

#ifdef WITH_DIRALIASES
        const int real_errno = errno;
        const char *where_alias;

        if ((where_alias = lookup_alias(where)) == NULL ||
            chdir(where_alias) != 0) {
            errno = real_errno;
        } else {
            goto chdir_success;
        }
#endif

        if (SNCHECK(snprintf(buffer, sizeof buffer,
                             MSG_CANT_CHANGE_DIR ": %s",
                             dir, strerror(errno)), sizeof buffer)) {
            _EXIT(EXIT_FAILURE);
        }
        logfile(LOG_INFO, "%s", buffer);
        addreply(550, "%s", buffer);

#ifndef MINIMAL
# ifndef NO_DIRSCAN_DELAY
        if (cwd_failures >= MAX_DIRSCAN_TRIES) {
            _EXIT(EXIT_FAILURE);
        }
        usleep2(cwd_failures * DIRSCAN_FAILURE_DELAY);
        cwd_failures++;
# endif
#endif

        return;
    }

#ifdef WITH_DIRALIASES
    chdir_success:
#endif

#ifndef MINIMAL
    cwd_failures = 0UL;
    dobanner(1);
#endif
    if (getcwd(wd, sizeof wd - (size_t) 1U) == NULL) {
        if (*dir == '/') {
            if (SNCHECK(snprintf(wd, sizeof wd, "%s", dir), sizeof wd)) { /* already checked */
                _EXIT(EXIT_FAILURE);
            }
        } else {
            const size_t dir_len = strlen(dir);
            const size_t wd_len = strlen(wd);
            if (sizeof wd < dir_len + sizeof "/" - 1U + wd_len + 1U) {
                kaboom:
                die(421, LOG_ERR, MSG_PATH_TOO_LONG);
            }
            strcat(strcat(wd, "/"), dir); /* safe, see above */
        }
    }
    addreply(250, MSG_CURRENT_DIR_IS, wd);
}