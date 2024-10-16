static char *url_encode(char *buf, size_t len, const char *input)
{
    char *output = buf;
    if (!input || !output || len == 0)
        return NULL;
    while (*input && --len)
    {
        if (!isascii(*input) || !isgraph(*input) || *input == '%')
        {
            if (len <= 2)
                break;
            *output++ = '%';
            char2hex(*input++, output);
            output += 2;
            len -= 2;
        }
        else
        {
            *output++ = *input++;
        }
    }
    *output = 0;
    return buf;
}