static void curl_parse_filename(const char *filename, QDict *options,
                                Error **errp)
{

    #define RA_OPTSTR ":readahead="
    char *file;
    char *ra;
    const char *ra_val;
    int parse_state = 0;

    file = g_strdup(filename);

    /* Parse a trailing ":readahead=#:" param, if present. */
    ra = file + strlen(file) - 1;
    while (ra >= file) {
        if (parse_state == 0) {
            if (*ra == ':') {
                parse_state++;
            } else {
                break;
            }
        } else if (parse_state == 1) {
            if (*ra > '9' || *ra < '0') {
                char *opt_start = ra - strlen(RA_OPTSTR) + 1;
                if (opt_start > file &&
                    strncmp(opt_start, RA_OPTSTR, strlen(RA_OPTSTR)) == 0) {
                    ra_val = ra + 1;
                    ra -= strlen(RA_OPTSTR) - 1;
                    *ra = '\0';
                    qdict_put(options, "readahead", qstring_from_str(ra_val));
                }
                break;
            }
        }
        ra--;
    }

    qdict_put(options, "url", qstring_from_str(file));

    g_free(file);
}