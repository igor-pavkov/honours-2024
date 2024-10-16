static void process_args(char *args, int pass)
{
    char *p, *q, *arg, *prevarg;
    char separator = ' ';

    p = args;
    if (*p && *p != '-')
        separator = *p++;
    arg = NULL;
    while (*p) {
        q = p;
        while (*p && *p != separator)
            p++;
        while (*p == separator)
            *p++ = '\0';
        prevarg = arg;
        arg = q;
        if (process_arg(prevarg, arg, pass))
            arg = NULL;
    }
    if (arg)
        process_arg(arg, NULL, pass);
}