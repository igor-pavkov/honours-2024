static int mmemcmp(const char *p, const char *q, size_t l, bool casesense)
{
    return casesense ? memcmp(p, q, l) : nasm_memicmp(p, q, l);
}