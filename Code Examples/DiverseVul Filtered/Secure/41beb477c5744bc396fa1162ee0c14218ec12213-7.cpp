char Far * Far zfstrcpy(char Far *s1, const char Far *s2)
{
    char Far *p = s1;

    while ((*s1++ = *s2++) != '\0');
    return p;
}