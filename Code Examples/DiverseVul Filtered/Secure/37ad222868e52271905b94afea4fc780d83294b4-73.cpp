static void displayrate(const char *word, off_t size,
                        const double started,
                        const char * const name, int up)
{
    double ended;
    double t;
    double speed;
    char speedstring[64];

    ended = get_usec_time();

    t = ended - started;
    if (t > 0.0 && size > (off_t) 0) {
        speed = size / t;
    } else {
        speed = 0.0;
    }
    if (speed > 524288.0) {
        addreply(0, MSG_TRANSFER_RATE_M, t, speed / 1048576.0);
    } else if (speed > 512.0) {
        addreply(0, MSG_TRANSFER_RATE_K, t, speed / 1024.0);
    } else if (speed > 0.1) {
        addreply(0, MSG_TRANSFER_RATE_B, t, speed);
    }
    if (!SNCHECK(snprintf(speedstring, sizeof speedstring,
                          " (%llu bytes, %.2fKB/sec)",
                          (unsigned long long) size, speed / 1024.0),
                 sizeof speedstring)) {
        logfile(LOG_NOTICE, "%s%s%s%s %s %s", root_directory,
                *name == '/' ? "" : wd,
                (*name != '/' && (!*wd || wd[strlen(wd) - 1] != '/'))
                ? "/" : "", name, word, speedstring);
    }
    /* Tons of #ifdef here, but it avoids a pointless call to realpath() */
#if defined(WITH_UPLOAD_SCRIPT) || defined(WITH_ALTLOG)
    if (
# ifdef WITH_ALTLOG
        altlog_format != ALTLOG_NONE
# endif
# if defined(WITH_UPLOAD_SCRIPT) && defined(WITH_ALTLOG)
        ||
# endif
# if (defined(WITH_UPLOAD_SCRIPT))
        (do_upload_script != 0 && up != 0)
# endif
        )
    {
        char *alloca_filename_real;
        const size_t sizeof_filename_real = PATH_MAX + VHOST_PREFIX_MAX_LEN;
        char *resolved_path;
        const size_t sizeof_resolved_path = PATH_MAX + 1U;

        if ((resolved_path = malloc(sizeof_resolved_path)) == NULL) {
            return;
        }
        resolved_path[sizeof_resolved_path - 1U] = 0;
        if (realpath(name, resolved_path) == NULL) {
            if (up != 0) {
                (void) unlink(name);
            }
            free(resolved_path);
            logfile(LOG_ERR, "realpath() failure : [%s] => [%s]",
                    name, strerror(errno));
            return;
        }
        if (resolved_path[sizeof_resolved_path - 1U] != 0) {
            for (;;) {
                *resolved_path++ = 0;
            }
        }
        if ((alloca_filename_real = ALLOCA(sizeof_filename_real)) == NULL) {
            free(resolved_path);
            return;
        }
# ifdef WITH_VIRTUAL_CHROOT
        if (SNCHECK(snprintf(alloca_filename_real, sizeof_filename_real,
                             "\001%s", resolved_path), sizeof_filename_real)) {
            goto rp_failure;
        }
# else
        if (SNCHECK(snprintf(alloca_filename_real, sizeof_filename_real,
                             "\001%s%s", root_directory,
                             (*resolved_path == '/' ? resolved_path + 1 :
                              resolved_path)), sizeof_filename_real)) {
            goto rp_failure;
        }
# endif
# ifdef WITH_ALTLOG
        (void) altlog_writexfer(up, alloca_filename_real + 1, size, t);
# endif
# if defined(WITH_UPLOAD_SCRIPT)
        if (do_upload_script != 0 && up != 0) {
            upload_pipe_push(account, alloca_filename_real);
        }
# endif
        rp_failure:
        free(resolved_path);
        ALLOCA_FREE(alloca_filename_real);
    }
#else
    (void) up;
#endif
}