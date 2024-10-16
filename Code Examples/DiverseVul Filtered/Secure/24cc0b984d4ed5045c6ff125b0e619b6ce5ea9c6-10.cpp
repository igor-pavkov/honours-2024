sds sdsResize(sds s, size_t size) {
    void *sh, *newsh;
    char type, oldtype = s[-1] & SDS_TYPE_MASK;
    int hdrlen, oldhdrlen = sdsHdrSize(oldtype);
    size_t len = sdslen(s);
    sh = (char*)s-oldhdrlen;

    /* Return ASAP if the size is already good. */
    if (sdsalloc(s) == size) return s;

    /* Truncate len if needed. */
    if (size < len) len = size;

    /* Check what would be the minimum SDS header that is just good enough to
     * fit this string. */
    type = sdsReqType(size);
    /* Don't use type 5, it is not good for strings that are resized. */
    if (type == SDS_TYPE_5) type = SDS_TYPE_8;
    hdrlen = sdsHdrSize(type);

    /* If the type is the same, or can hold the size in it with low overhead
     * (larger than SDS_TYPE_8), we just realloc(), letting the allocator
     * to do the copy only if really needed. Otherwise if the change is
     * huge, we manually reallocate the string to use the different header
     * type. */
    if (oldtype==type || (type < oldtype && type > SDS_TYPE_8)) {
        newsh = s_realloc(sh, oldhdrlen+size+1);
        if (newsh == NULL) return NULL;
        s = (char*)newsh+oldhdrlen;
    } else {
        newsh = s_malloc(hdrlen+size+1);
        if (newsh == NULL) return NULL;
        memcpy((char*)newsh+hdrlen, s, len);
        s_free(sh);
        s = (char*)newsh+hdrlen;
        s[-1] = type;
    }
    s[len] = 0;
    sdssetlen(s, len);
    sdssetalloc(s, size);
    return s;
}