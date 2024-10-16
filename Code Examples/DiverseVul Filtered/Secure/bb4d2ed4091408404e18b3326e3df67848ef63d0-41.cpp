int name_cmp(const char *name, const char *cmp)
{
    int len, ret;
    char c;
    len = strlen(cmp);
    if ((ret = strncmp(name, cmp, len)))
        return ret;
    c = name[len];
    if (!c || (c == '.'))
        return 0;
    return 1;
}