int Far zfstrcmp(const char Far *s1, const char Far *s2)
{
    int ret;

    while ((ret = (int)(uch)*s1 - (int)(uch)*s2) == 0
           && *s2 != '\0') {
        ++s2; ++s1;
    }
    return ret;
}