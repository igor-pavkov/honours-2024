void am_strip_blank(const char **s)
{
    while ((**s == ' ') || (**s == '\t'))
        (*s)++;
    return;
}