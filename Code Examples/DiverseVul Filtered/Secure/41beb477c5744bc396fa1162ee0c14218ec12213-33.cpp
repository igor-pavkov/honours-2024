unsigned char *uzmbsrchr(str, c)
    ZCONST unsigned char *str;
    unsigned int c;
{
    unsigned char *match = NULL;
    while(*str != '\0'){
        if (*str == c) {match = (unsigned char *)str;}
        INCSTR(str);
    }
    return match;
}