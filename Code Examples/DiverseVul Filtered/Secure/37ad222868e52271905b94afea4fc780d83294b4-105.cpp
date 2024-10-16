void simplify(char *subdir)
{
    char *a;

    if (subdir == NULL || *subdir == 0) {
        return;
    }
    while ((a = strstr(subdir, "//")) != NULL) {
        overlapcpy(a, a + 1);
    }
    while ((a = strstr(subdir, "/./")) != NULL) {
        overlapcpy(a, a + 2);
    }
    while (strncmp(subdir, "../", 3) == 0) {
        subdir += 3;
    }
    a = strstr(subdir, "/../");
    if (a != NULL) {
        if (a == subdir) {
            while (strncmp(subdir, "/../", 4) == 0) {
                overlapcpy(subdir, subdir + 3);
            }
            a = strstr(subdir, "/../");
        }
        while (a != NULL) {
            char *nextcomponent = a + 4;
            if (a != subdir && *a == '/') {
                a--;
            }
            while (a != subdir && *a != '/') {
                a--;
            }
            if (*a == '/') {
                a++;
            }
            overlapcpy(a, nextcomponent);
            a = strstr(subdir, "/../");
        }
    }
    a = subdir;
    if (*a == '.') {
        a++;
        if (*a == 0) {
            return;
        }
        if (*a == '/') {
            while (*a == '/') {
                a++;
            }
            overlapcpy(subdir, a);
        }
    }
    if (*a == 0) {
        return;
    }
    a = subdir + strlen(subdir) - (size_t) 1U;
    if (*a != '.' || a == subdir) {
        return;
    }
    a--;
    if (*a == '/') {
        a[1] = 0;
        return;
    }
    if (*a != '.' || a == subdir) {
        return;
    }
    a--;
    if (*a != '/') {
        return;
    }
    *a = 0;
    if ((a = strrchr(subdir, '/')) == NULL) {
        *subdir = '/';
        subdir[1] = 0;
        return;
    }
    a[1] = 0;
}