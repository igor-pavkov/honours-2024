static int check_newline(const char *progname, const char *name)
{
    char *s;
    for (s = "\n"; *s; s++) {
        if (strchr(name, *s)) {
            fprintf(stderr, "%s: illegal character 0x%02x in mount entry\n",
                    progname, *s);
            return EX_USAGE;
        }
    }
    return 0;
}