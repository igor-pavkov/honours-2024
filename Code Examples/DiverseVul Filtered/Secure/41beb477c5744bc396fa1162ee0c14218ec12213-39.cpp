int zstrnicmp(s1, s2, n)
    register ZCONST char *s1, *s2;
    register unsigned n;
{
    for (; n > 0;  --n, ++s1, ++s2) {

        if (ToLower(*s1) != ToLower(*s2))
            /* test includes early termination of one string */
            return ((uch)ToLower(*s1) < (uch)ToLower(*s2))? -1 : 1;

        if (*s1 == '\0')   /* both strings terminate early */
            return 0;
    }
    return 0;
}