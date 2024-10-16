void douser(const char *username)
{
    struct passwd *pw = NULL;

    if (loggedin) {
        if (username) {
            if (!guest) {
                addreply_noformat(530, MSG_ALREADY_LOGGED);
            } else if (broken_client_compat != 0) {
                addreply_noformat(331, MSG_ANY_PASSWORD);
            } else {
                addreply_noformat(230, MSG_ANONYMOUS_LOGGED);
                dot_read_ok = dot_read_anon_ok;
                dot_write_ok = 0;
            }
        }
        return;
    }
    if (anon_only <= 0 && username != NULL && *username != 0 &&
        (anon_only < 0 || (strcasecmp(username, "ftp") &&
                           strcasecmp(username, "anonymous")))) {
        strncpy(account, username, sizeof(account) - 1);
        account[sizeof(account) - (size_t) 1U] = 0;
        addreply(331, MSG_USER_OK, account);
        loggedin = 0;
    } else if (anon_only < 0) {
        if (broken_client_compat != 0) {
            addreply(331, MSG_USER_OK, username);
            return;
        } else {
            die(530, LOG_DEBUG, MSG_NO_ANONYMOUS_LOGIN);
        }
    } else {
#ifdef WITH_VIRTUAL_HOSTS
        char name[PATH_MAX];
        char hbuf[NI_MAXHOST];
#endif
        if (chrooted != 0) {
            die(421, LOG_DEBUG, MSG_CANT_DO_TWICE);
        }

#ifdef PER_USER_LIMITS
        if (per_anon_max > 0U && ftpwho_read_count("ftp") >= per_anon_max) {
            addreply(421, MSG_PERUSER_MAX, (unsigned long) per_anon_max);
            doreply();
            _EXIT(1);
        }
#endif

#ifdef NON_ROOT_FTP
        {
            static struct passwd pw_;
            char s[PATH_MAX + 1U];

            if (getcwd(s, sizeof s - (size_t) 1U) == NULL) {
                cantsec:
                die(421, LOG_ERR, MSG_UNABLE_SECURE_ANON);
            }
            pw_.pw_uid = geteuid();
            pw_.pw_gid = getegid();
            pw_.pw_dir = (char *) NON_ROOT_ANON_DIR;
            if (home_directory != NULL) {
                pw_.pw_dir = (char *) home_directory;
            }
            if (getenv("FTP_ANON_DIR") != NULL) {
                pw_.pw_dir = getenv("FTP_ANON_DIR");
            }
            if (pw_.pw_dir == NULL) {
                pw_.pw_dir = strdup(s);    /* checked for == NULL later */
            }
            pw = &pw_;
        }
#else
        if (((pw = getpwnam("ftp")) == NULL &&
             (pw = getpwnam("_ftp")) == NULL) ||
            pw->pw_uid == 0 || pw->pw_gid == 0 ||
            (doinitsupgroups("ftp", (uid_t) -1, pw->pw_gid) != 0 &&
             doinitsupgroups("_ftp", (uid_t) -1, pw->pw_gid) != 0) ||
            setgid(pw->pw_gid) || setegid(pw->pw_gid)) {
            cantsec:
            die(421, LOG_ERR, MSG_UNABLE_SECURE_ANON);
        }
# ifdef ANON_DIR
        if ((pw->pw_dir = strdup(ANON_DIR)) == NULL) {
            die_mem();
        }
# endif
#endif
#ifdef WITH_VIRTUAL_HOSTS
        if (getnameinfo((struct sockaddr *) &ctrlconn, STORAGE_LEN(ctrlconn),
                        hbuf, sizeof hbuf, NULL,
                        (size_t) 0U, NI_NUMERICHOST) != 0
            || SNCHECK(snprintf(name, sizeof name, VHOST_PATH "/%s", hbuf),
                       sizeof name)) {
            _EXIT(EXIT_FAILURE);
        }
        if (chdir(name) != 0)         /* non-virtual */
#endif
        {
            char *hd;
            size_t rd_len;

            if (pw->pw_dir == NULL || *pw->pw_dir != '/') {
                goto cantsec;
            }
            if ((hd = strstr(pw->pw_dir, "/./")) != NULL) {
                rd_len = (size_t) (hd - pw->pw_dir) + sizeof "/";
                if ((root_directory = malloc(rd_len)) == NULL) {
                    goto cantsec;
                }
                memcpy(root_directory, pw->pw_dir, rd_len);
                root_directory[rd_len - (size_t) 1U] = 0;
                hd += 2;
            } else {
                rd_len = strlen(pw->pw_dir) + sizeof "/";
                if ((root_directory = malloc(rd_len)) == NULL) {
                    goto cantsec;
                }
                snprintf(root_directory, rd_len, "%s/", pw->pw_dir);
                hd = (char *) "/";
            }
            if (chdir(root_directory) || chroot(root_directory) || chdir(hd)) {
                die(421, LOG_ERR, MSG_CANT_CHANGE_DIR " [%s]", root_directory, hd);
            }
            logfile(LOG_INFO, MSG_ANONYMOUS_LOGGED);
        }
#ifdef WITH_VIRTUAL_HOSTS
        else {                       /* virtual host */
            const size_t rd_len = strlen(hbuf) + sizeof ":/";

            if ((root_directory = malloc(rd_len)) == NULL ||
                chdir(name) || chroot(name) || chdir("/") ||
                SNCHECK(snprintf(root_directory, rd_len, "%s:/", hbuf),
                        rd_len)) {
                goto cantsec;
            }
            logfile(LOG_INFO, MSG_ANONYMOUS_LOGGED_VIRTUAL ": %s", hbuf);
        }
#endif
        if (pw == NULL) {
            goto cantsec;
        }
        chrooted = 1;
        authresult.uid = pw->pw_uid;
        authresult.gid = pw->pw_gid;
        if ((authresult.dir = strdup(pw->pw_dir)) == NULL) {
            die_mem();
        }

#ifdef THROTTLING
        if (throttling != 0) {
            addreply_noformat(0, MSG_BANDWIDTH_RESTRICTED);
            (void) nice(NICE_VALUE);
        } else {
            throttling_delay = throttling_bandwidth_ul =
                throttling_bandwidth_dl = 0UL;
        }
#endif

#ifndef NON_ROOT_FTP
        if (authresult.uid > (uid_t) 0) {
# ifndef WITHOUT_PRIVSEP
            if (setuid(authresult.uid) != 0 || seteuid(authresult.uid) != 0) {
                goto cantsec;
            }
# else
            if (seteuid(authresult.uid) != 0) {
                goto cantsec;
            }
#  ifdef USE_CAPABILITIES
            drop_login_caps();
#  endif
# endif
        }
#endif

#ifndef MINIMAL
        dobanner(0);
#endif

        if (broken_client_compat != 0) {
            addreply_noformat(331, MSG_ANONYMOUS_ANY_PASSWORD);
        } else {
            addreply_noformat(230, MSG_ANONYMOUS_LOGGED);
        }
        dot_write_ok = 0;
        dot_read_ok = dot_read_anon_ok;
        strncpy(account, "ftp", sizeof account - (size_t) 1U);
        account[(sizeof account) - 1U] = 0;
#ifdef FTPWHO
        if (shm_data_cur != NULL) {
            ftpwho_lock();
            strncpy(shm_data_cur->account, account,
                    sizeof shm_data_cur->account - (size_t) 1U);
            shm_data_cur->account[sizeof shm_data_cur->account - 1U] = 0;
            ftpwho_unlock();
            state_needs_update = 1;
        }
#endif
        loggedin = guest = 1;
#ifdef QUOTAS
        user_quota_size = user_quota_files = ULONG_LONG_MAX;
#endif
    }
    if (getcwd(wd, sizeof wd - (size_t) 1U) == NULL) {
        wd[0] = '/';
        wd[1] = 0;
    }
#ifdef WITH_BONJOUR
    refreshManager();
#endif
}