void sdssubstr(sds s, size_t start, size_t len) {
    /* Clamp out of range input */
    size_t oldlen = sdslen(s);
    if (start >= oldlen) start = len = 0;
    if (len > oldlen-start) len = oldlen-start;

    /* Move the data */
    if (len) memmove(s, s+start, len);
    s[len] = 0;
    sdssetlen(s,len);
}