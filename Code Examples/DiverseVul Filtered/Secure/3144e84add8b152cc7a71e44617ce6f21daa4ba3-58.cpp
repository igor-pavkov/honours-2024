static int mstrcmp(const char *p, const char *q, bool casesense)
{
    return casesense ? strcmp(p, q) : nasm_stricmp(p, q);
}