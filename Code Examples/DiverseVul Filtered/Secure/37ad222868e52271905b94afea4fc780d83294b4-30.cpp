void dornfr(char *name)
{
    struct stat st;

#ifndef ANON_CAN_RENAME
    if (guest != 0) {
        addreply_noformat(550, MSG_ANON_CANT_RENAME);
        return;
    }
#endif
    if (disallow_rename != 0) {
        addreply_noformat(550, MSG_RENAME_FAILURE);
        return;
    }
    if (checknamesanity(name, dot_write_ok) != 0) {
        addreply(553, MSG_SANITY_FILE_FAILURE, name);
        return;
    }
    if ((lstat(name, &st)) == 0) {
        if (renamefrom != NULL) {
            addreply_noformat(0, MSG_RENAME_ABORT);
            (void) free(renamefrom);
        }
        if ((renamefrom = strdup(name)) == NULL) {
            die_mem();
        }
        addreply_noformat(350, MSG_RENAME_RNFR_SUCCESS);
    } else {
        addreply_noformat(550, MSG_FILE_DOESNT_EXIST);
    }
}