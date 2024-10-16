void dornto(char *name)
{
#ifdef QUOTAS
    off_t target_file_size = (off_t) -1;
    int files_count = 0;
    long long bytes = 0LL;
#endif

#ifndef ANON_CAN_RENAME
    if (guest != 0) {
        addreply_noformat(550, MSG_ANON_CANT_RENAME);
        goto bye;
    }
#endif
    if (renamefrom == NULL) {
        addreply_noformat(503, MSG_RENAME_NORNFR);
        goto bye;
    }
    if (checknamesanity(name, dot_write_ok) != 0) {
        addreply(553, MSG_SANITY_FILE_FAILURE, name);
        return;                        /* don't clear rnfrom buffer */
    }
#ifdef QUOTAS
    if (hasquota() == 0) {
        struct stat st_source, st_target;

        if (stat(renamefrom, &st_source) != 0) {
            addreply_noformat(550, MSG_RENAME_FAILURE);
            goto bye;
        }
        if (stat(name, &st_target) != 0) {
            if (errno == ENOENT) {
                target_file_size = (off_t) -1;
            } else {
                addreply_noformat(550, MSG_RENAME_FAILURE);
                goto bye;
            }
        } else if (st_source.st_ino == st_target.st_ino &&
                   st_source.st_dev == st_target.st_dev) {
            addreply_noformat(250, MSG_RENAME_SUCCESS);
            goto bye;
        } else {
            target_file_size = st_target.st_size;
        }
        if (target_file_size >= (off_t) 0) {
            bytes = - (long long) target_file_size;
            files_count = -1;
            (void) quota_update(NULL, files_count, bytes, NULL);
        } else {
            bytes = (off_t) 0;
        }
    }
#endif
    if (rename(renamefrom, name) < 0) {
        error(451, MSG_RENAME_FAILURE);
#ifdef QUOTAS
        (void) quota_update(NULL, -files_count, -bytes, NULL);
#endif
    } else {
        addreply_noformat(250, MSG_RENAME_SUCCESS);
        logfile(LOG_NOTICE, MSG_RENAME_SUCCESS ": [%s]->[%s]",
                renamefrom, name);
    }
    bye:
    (void) free(renamefrom);
    renamefrom = NULL;
}