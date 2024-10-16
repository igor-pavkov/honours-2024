static char *url_decode(char *buf, size_t len, const char *input)
{
    char *output = buf;
    if (!input || !output || len == 0)
        return NULL;

    while (*input && --len)
    {
        if (*input == '%')
        {
            if (is_hexdigit(input[1]) && is_hexdigit(input[2]))
            {
                *output++ = hex2char(++input);
                input += 2;
            }
            else
            {
                return 0;
            }
        }
        else
        {
            *output++ = *input++;
        }
    }
    *output = 0;
    return buf;
}