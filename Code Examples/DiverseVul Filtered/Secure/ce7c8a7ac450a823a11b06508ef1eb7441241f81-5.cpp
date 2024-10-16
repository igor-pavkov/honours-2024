char *_q_fgetline(FILE *fp, size_t initsize)
{
    size_t memsize = initsize;

    char *str = (char *)malloc(memsize);
    if (str == NULL) return NULL;

    char *ptr;
    size_t readsize;
    for (ptr = str, readsize = 0; ;) {
        int c = fgetc(fp);
        if (c == EOF) {
            if (readsize == 0) {
                free(str);
                return NULL;
            }
            break;
        }

        *ptr++ = (char)c;
        readsize++;
        if (readsize == memsize) {
            memsize *= 2;
            char *strtmp = (char *)malloc(memsize);
            if (strtmp == NULL) {
                free(str);
                return NULL;
            }

            memcpy(strtmp, str, readsize);
            free(str);
            str = strtmp;
            ptr = str + readsize;
        }

        if (c == '\n') break;
    }

    *ptr = '\0';

    return str;
}