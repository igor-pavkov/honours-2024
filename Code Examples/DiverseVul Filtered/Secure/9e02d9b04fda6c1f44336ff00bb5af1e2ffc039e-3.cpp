static int isAlphaStr(const char *str)
{
    char c;

    c = str[0];
    while (c) 
    {
        if (isAlpha(c) == 0) return 0;
        str ++;
        c = str[0];
    }

    return 1;
}