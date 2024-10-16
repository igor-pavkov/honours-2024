static int create_home_and_chdir(const char * const home)
{
    char *pathcomp;
    char *z;
    size_t len;
    const char delim = '/';

    if (home == NULL || *home != '/') {
        return -1;
    }
    if (chdir(home) == 0) {
        return 0;
    }
    if (create_home == 0) {
        return -1;
    }
    len = strlen(home) + (size_t) 1U;
    if (len < (size_t) 2U || *home != delim) {
        return -1;
    }
    if ((pathcomp = ALLOCA(len)) == NULL) {
        return -1;
    }
    memcpy(pathcomp, home, len);       /* safe, no possible overflow */
    z = pathcomp;
    for (;;) {
        z++;
        if (*z == 0) {
            break;
        }
        if (*z == delim) {
            *z = 0;
            if (z[1] == 0) {
                break;
            }
            (void) mkdir(pathcomp, (mode_t) 0755);
            *z = delim;
        }
    }
    ALLOCA_FREE(pathcomp);
    (void) mkdir(home, (mode_t) 0700);
    if (chdir(home) != 0) {
        return -1;
    }
    if (chmod(home, (mode_t) 0777 & ~u_mask_d) < 0 ||
        chown(home, authresult.uid, authresult.gid) < 0) {
        return -1;
    }

    return chdir(home);
}