int zstat(p, s)
    ZCONST char *p;
    struct stat *s;
{
    return (stat((char *)p,s) >= 0? 0 : (-1));
}