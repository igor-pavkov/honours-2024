static int tryautorename(const char * const atomic_file, char * const name,
                         const char ** const name2_)
{
    static char name2[PATH_MAX];
    unsigned int gc = 0U;

    if (link(atomic_file, name) == 0) {
        *name2_ = NULL;
        (void) unlink(atomic_file);
        return 0;
    }
    *name2_ = name2;
    for (;;) {
        gc++;
        if (gc == 0U ||
#ifdef AUTORENAME_REVERSE_ORDER
            SNCHECK(snprintf(name2, sizeof name2, "%u.%s", gc, name),
                    sizeof name2)
#else
            SNCHECK(snprintf(name2, sizeof name2, "%s.%u", name, gc),
                    sizeof name2)
#endif
            ) {
            break;
        }
        if (link(atomic_file, name2) == 0) {
            (void) unlink(atomic_file);
            return 0;
        }
        switch (errno) {
#ifdef EEXIST
        case EEXIST:
#endif
#ifdef EISDIR
        case EISDIR:
#endif
#ifdef ETXTBSY
        case ETXTBSY:
#endif
            continue;
        }
        break;
    }
    *name2_ = NULL;

    return -1;
}