void domkd(char *name)
{
#ifdef QUOTAS
    Quota quota;
    int overflow;
#endif

    if (guest != 0 && allow_anon_mkdir == 0) {
        addreply_noformat(550, MSG_ANON_CANT_MKD);
        return;
    }
    if (checknamesanity(name, dot_write_ok) != 0) {
        addreply_noformat(553, MSG_SANITY_DIRECTORY_FAILURE);
        return;
    }
#ifdef QUOTAS
    if (quota_update(&quota, 1LL, 0LL, &overflow) == 0 && overflow != 0) {
        (void) quota_update(&quota, -1LL, 0LL, NULL);
        addreply(552, MSG_QUOTA_EXCEEDED, name);
        goto end;
    }
#endif
    if ((mkdir(name, (mode_t) (0777 & ~u_mask_d))) < 0) {
#ifdef QUOTAS
        (void) quota_update(&quota, -1LL, 0LL, NULL);
#endif
        error(550, MSG_MKD_FAILURE);
    } else {
        addreply(257, "\"%s\" : " MSG_MKD_SUCCESS, name);
#ifndef MINIMAL
        cwd_failures = 0UL;
#endif
    }
#ifdef QUOTAS
    end:
    displayquota(&quota);
#endif
}