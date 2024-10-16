inline char * qs_scanvalue(const char * key, const char * qs, char * val, size_t val_len)
{
    size_t i, key_len;
    const char * tmp;

    // find the beginning of the k/v substrings
    if ( (tmp = strchr(qs, '?')) != NULL )
        qs = tmp + 1;

    key_len = strlen(key);
    while(qs[0] != '#' && qs[0] != '\0')
    {
        if ( qs_strncmp(key, qs, key_len) == 0 )
            break;
        qs += strcspn(qs, "&") + 1;
    }

    if ( qs[0] == '\0' ) return NULL;

    qs += strcspn(qs, "=&#");
    if ( qs[0] == '=' )
    {
        qs++;
        i = strcspn(qs, "&=#");
#ifdef _MSC_VER
        strncpy_s(val, val_len, qs, (val_len - 1)<(i + 1) ? (val_len - 1) : (i + 1));
#else
        strncpy(val, qs, (val_len - 1)<(i + 1) ? (val_len - 1) : (i + 1));
#endif
		qs_decode(val);
    }
    else
    {
        if ( val_len > 0 )
            val[0] = '\0';
    }

    return val;
}