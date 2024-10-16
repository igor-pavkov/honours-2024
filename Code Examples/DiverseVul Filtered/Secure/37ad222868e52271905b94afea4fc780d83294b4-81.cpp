void dormd(char *name)
{
#ifdef QUOTAS
    Quota quota;
#endif

#ifndef ANON_CAN_DELETE
    if (guest != 0) {
        addreply_noformat(550, MSG_ANON_CANT_RMD);
        return;
    }
#endif
    if (checknamesanity(name, dot_write_ok) != 0) {
        addreply_noformat(553, MSG_SANITY_DIRECTORY_FAILURE);
        return;
    }
    if ((rmdir(name)) < 0) {
        error(550, MSG_RMD_FAILURE);
    } else {
#ifdef QUOTAS
        if (quota_update(&quota, -1LL, 0LL, NULL) == 0) {
            displayquota(&quota);
        }
#endif
        addreply_noformat(250, MSG_RMD_SUCCESS);
    }
}