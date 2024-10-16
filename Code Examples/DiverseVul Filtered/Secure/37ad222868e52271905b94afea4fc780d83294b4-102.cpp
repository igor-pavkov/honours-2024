static char *get_atomic_file(const char * const file)
{
    static char res[PATH_MAX];
    char *z;
    size_t orig_len;
    size_t slash;
    size_t sizeof_atomic_prefix;

    if (file == NULL) {
        return res;
    }
    if ((z = strrchr(file, '/')) == NULL) {
        *res = 0;
        orig_len = (size_t) 0U;
    } else {
        slash = (size_t) (z - file);
        if (slash >= (sizeof res - (size_t) 1U)) {
            return NULL;
        }
        slash++;
        if (file[slash] == 0) {
            return NULL;
        }
        strncpy(res, file, slash);
        res[slash] = 0;
        orig_len = strlen(res);
    }
    sizeof_atomic_prefix = strlen(atomic_prefix) + (size_t) 1U;
    if (sizeof res - orig_len < sizeof_atomic_prefix) {
        return NULL;
    }
    memcpy(res + orig_len, atomic_prefix, sizeof_atomic_prefix);

    return res;
}