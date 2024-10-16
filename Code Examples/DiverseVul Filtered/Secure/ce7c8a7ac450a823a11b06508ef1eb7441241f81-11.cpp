char *_q_fgets(char *str, size_t size, FILE *fp)
{
    int c;
    char *ptr;

    for (ptr = str; size > 1; size--) {
        c = fgetc(fp);
        if (c == EOF) break;
        *ptr++ = (char)c;
        if (c == '\n') break;
    }

    *ptr = '\0';
    if (strlen(str) == 0) return NULL;

    return str;
}