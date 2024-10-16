void dopass(char *password)
{
    static unsigned int tapping;
    char *hd;
#if !defined(MINIMAL) && defined(HAVE_GETGROUPS) && defined(DISPLAY_GROUPS)
    gid_t *groups = NULL;
    int ngroups;
# if defined(NGROUPS_MAX) && NGROUPS_MAX > 0
    int ngroups_max = NGROUPS_MAX; /* Use the compile time value */
# else
    int ngroups_max = 1; /* use a sane default */
# endif
#endif

    if (loggedin != 0) {
        if (guest != 0) {
            addreply_noformat(230, MSG_NO_PASSWORD_NEEDED);
#ifdef LOG_ANON_EMAIL
            snprintf(account, sizeof account, "ftp: <%s> ", password);
#endif
        } else {
            addreply_noformat(530, MSG_CANT_DO_TWICE);
        }
        return;
    }
    if (*account == 0) {
        addreply_noformat(530, MSG_WHOAREYOU);
        return;
    }
    if (strlen(password) >= MAX_PASSWORD_LEN) {
        addreply_noformat(530, MSG_LINE_TOO_LONG);
        return;
    }
    authresult = pw_check(account, password, &ctrlconn, &peer);
    pure_memzero(password, strlen(password));
    if (authresult.auth_ok != 1) {
        tapping++;
        randomsleep(tapping);
        addreply_noformat(530, MSG_AUTH_FAILED);
        doreply();
        if (tapping > MAX_PASSWD_TRIES) {
            logfile(LOG_ERR, MSG_AUTH_TOOMANY);
            _EXIT(EXIT_FAILURE);
        }
        logfile(LOG_WARNING, MSG_AUTH_FAILED_LOG, account);
        return;
    }
    if (authresult.uid < useruid) {
        logfile(LOG_WARNING, MSG_ACCOUNT_DISABLED " (uid < %lu)",
                account, (unsigned long) useruid);
        randomsleep(tapping);
        if (tapping >= MAX_PASSWD_TRIES) {
            addreply_noformat(530, MSG_AUTH_FAILED);
            doreply();
            _EXIT(EXIT_FAILURE);
        }
        addreply_noformat(530, MSG_NOTRUST);
        doreply();
        return;
    }

#ifdef PER_USER_LIMITS
    if (per_user_max > 0U && ftpwho_read_count(account) >= per_user_max) {
        addreply(421, MSG_PERUSER_MAX, (unsigned long) per_user_max);
        doreply();
        _EXIT(1);
    }
#endif

    /* Add username and primary group to the uid/gid cache */
    (void) getname(authresult.uid);
    (void) getgroup(authresult.gid);

    if (
#if defined(WITH_LDAP) || defined(WITH_MYSQL) || defined(WITH_PGSQL) || defined(WITH_PUREDB) || defined(WITH_EXTAUTH)
        doinitsupgroups(NULL, authresult.uid, authresult.gid) != 0
#else
        doinitsupgroups(account, (uid_t) -1, authresult.gid) != 0
#endif
        ) {
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__CYGWIN__)
        (void) 0;
#else
        die(421, LOG_WARNING, MSG_NOTRUST);
#endif
    }

    /* handle /home/user/./public_html form */
    if ((root_directory = strdup(authresult.dir)) == NULL) {
        die_mem();
    }
    hd = strstr(root_directory, "/./");
    if (hd != NULL) {
        if (chrooted != 0) {
            die(421, LOG_DEBUG, MSG_CANT_DO_TWICE);
        }
        if (create_home_and_chdir(root_directory)) {
            die(421, LOG_ERR, MSG_NO_HOMEDIR);
        }
        *++hd = 0;
        hd++;
        if (chroot(root_directory) || chdir(hd)) {
            die(421, LOG_ERR, MSG_NO_HOMEDIR);
        }
        chrooted = 1;
#ifdef RATIOS
        if (ratio_for_non_anon == 0) {
            ratio_upload = ratio_download = 0U;
        }
        if (check_trustedgroup(authresult.uid, authresult.gid) != 0) {
            dot_write_ok = dot_read_ok = 1;
            ratio_upload = ratio_download = 0U;
            keepallfiles = 0;
        }
#endif
    } else {
        (void) free(root_directory);
        root_directory = (char *) "/";
        if (create_home_and_chdir(authresult.dir)) {
            die(421, LOG_ERR, MSG_NO_HOMEDIR);
        }
    }
    if (getcwd(wd, sizeof wd - (size_t) 1U) == NULL) {
        wd[0] = '/';
        wd[1] = 0;
    }
#ifndef NON_ROOT_FTP
    if (setgid(authresult.gid) || setegid(authresult.gid)) {
        _EXIT(EXIT_FAILURE);
    }
#endif
    if (check_trustedgroup(authresult.uid, authresult.gid) != 0) {
        userchroot = 0;
        dot_write_ok = dot_read_ok = 1;
        keepallfiles = 0;
#ifdef RATIOS
        ratio_upload = ratio_download = 0U;
#endif
#ifdef QUOTAS
        user_quota_files = user_quota_size = ULONG_LONG_MAX;
#endif
    }
#ifdef QUOTAS
    if (hasquota() == 0) {
        userchroot = 1;
    }
#endif
    if (loggedin == 0) {
        candownload = 1;        /* real users can always download */
    }
#ifdef THROTTLING
    if ((throttling == 2) || (guest != 0 && throttling == 1)) {
        addreply_noformat(0, MSG_BANDWIDTH_RESTRICTED);
        (void) nice(NICE_VALUE);
    } else {
        throttling_delay = throttling_bandwidth_dl =
            throttling_bandwidth_ul = 0UL;
    }
#endif
#if !defined(MINIMAL) && defined(HAVE_GETGROUPS) && defined(DISPLAY_GROUPS)
# ifdef SAFE_GETGROUPS_0
    ngroups = getgroups(0, NULL);
    if (ngroups > ngroups_max) {
        ngroups_max = ngroups;
    }
# elif defined(_SC_NGROUPS_MAX)
    /* get the run time value */
    ngroups = (int) sysconf(_SC_NGROUPS_MAX);
    if (ngroups > ngroups_max) {
        ngroups_max = ngroups;
    }
# endif
    if ((groups = malloc(sizeof(GETGROUPS_T) * ngroups_max)) == NULL) {
        die_mem();
    }
    ngroups = getgroups(ngroups_max, groups);
    if (guest == 0 && ngroups > 0) {
        char reply[80 + MAX_USER_LENGTH];
        const char *q;
        size_t p;

        if (SNCHECK(snprintf(reply, sizeof reply,
                             MSG_USER_GROUP_ACCESS ": ", account),
                    sizeof reply)) {
            _EXIT(EXIT_FAILURE);
        }
        p = strlen(reply);
        do {
            ngroups--;
            if ((ngroups != 0 && groups[ngroups] == groups[0]) ||
                (q = getgroup(groups[ngroups])) == NULL) {
                continue;
            }
            if (p + strlen(q) > 75) {
                reply[p] = 0;
                addreply(0, "%s", reply);
                *reply = 0;
                p = (size_t) 0U;
            }
            reply[p++] = ' ';
            while (*q != 0 && p < sizeof reply - (size_t) 1U) {
                reply[p++] = *q++;
            }
        } while (ngroups > 0);
        reply[p] = 0;
        addreply(0, "%s", reply);
    }
    free(groups);
#endif
    if (guest == 0 && allowfxp == 1) {
        addreply_noformat(0, MSG_FXP_SUPPORT);
    }
#ifdef RATIOS
    if (ratio_for_non_anon != 0 && ratio_upload > 0) {
        addreply(0, MSG_RATIO, ratio_upload, ratio_download);
    }
#endif
    if (userchroot != 0 && chrooted == 0) {
        if (chdir(wd) || chroot(wd)) {    /* should never fail */
            die(421, LOG_ERR, MSG_CHROOT_FAILED);
        }
        chrooted = 1;
#ifdef RATIOS
        if (ratio_for_non_anon == 0) {
            ratio_upload = ratio_download = 0U;
        }
#endif
        {
            const size_t rd_len = strlen(wd) + sizeof "/";

            if ((root_directory = malloc(rd_len)) == NULL) {
                die_mem();
            }
            snprintf(root_directory, rd_len, "%s/", wd);
        }
        wd[0] = '/';
        wd[1] = 0;
        if (chdir(wd)) {
            _EXIT(EXIT_FAILURE);
        }
        addreply(230, MSG_CURRENT_RESTRICTED_DIR_IS, wd);
    } else {
        addreply(230, MSG_CURRENT_DIR_IS, wd);
    }

#ifndef NON_ROOT_FTP
    disablesignals();
# ifndef WITHOUT_PRIVSEP
    if (setuid(authresult.uid) != 0 || seteuid(authresult.uid) != 0) {
        _EXIT(EXIT_FAILURE);
    }
# else
    if (seteuid(authresult.uid) != 0) {
        _EXIT(EXIT_FAILURE);
    }
#  ifdef USE_CAPABILITIES
    drop_login_caps();
#  endif
# endif
    enablesignals();
#endif
    logfile(LOG_INFO, MSG_IS_NOW_LOGGED_IN, account);
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
    loggedin = 1;
    if (getcwd(wd, sizeof wd - (size_t) 1U) == NULL) {
        wd[0] = '/';
        wd[1] = 0;
    }
#ifndef MINIMAL
    dobanner(0);
#endif
#ifdef QUOTAS
    displayquota(NULL);
#endif
#ifdef WITH_BONJOUR
    refreshManager();
#endif
}