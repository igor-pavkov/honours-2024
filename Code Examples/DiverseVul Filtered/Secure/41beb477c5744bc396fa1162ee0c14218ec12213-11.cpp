unsigned char *uzmbschr(str, c)
    ZCONST unsigned char *str;
    unsigned int c;
{
    while(*str != '\0'){
        if (*str == c) {return (unsigned char *)str;}
        INCSTR(str);
    }
    return NULL;
}