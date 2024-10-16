static int get_key_number(uint32_t *ret, const buffer *b) {
    const char * const s = b->ptr;
    size_t j = 0;
    unsigned long n;
    uint32_t sp = 0;
    char tmp[10 + 1]; /* #define UINT32_MAX_STRLEN 10 */

    for (size_t i = 0, used = buffer_clen(b); i < used; ++i) {
        if (light_isdigit(s[i])) {
            tmp[j] = s[i];
            if (++j >= sizeof(tmp)) return -1;
        }
        else if (s[i] == ' ') ++sp; /* count num spaces */
    }
    tmp[j] = '\0';
    n = strtoul(tmp, NULL, 10);
    if (n > UINT32_MAX || 0 == sp || !light_isdigit(*tmp)) return -1;
    *ret = (uint32_t)n / sp;
    return 0;
}