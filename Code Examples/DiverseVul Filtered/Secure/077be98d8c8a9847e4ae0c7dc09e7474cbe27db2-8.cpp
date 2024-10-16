static void handle_forward(srs_t *srs, FILE *fp, const char *address,
                           const char *domain, const char **excludes)
{
    int result;
    size_t addrlen;
    char value[1024];
    char outputbuf[1024], *output;

    addrlen = strlen(address);
    for (; *excludes; excludes++)
    {
        size_t len;
        len = strlen(*excludes);
        if (len >= addrlen)
            continue;
        if (strcasecmp(*excludes, &address[addrlen - len]) == 0
            && (**excludes == '.' || address[addrlen - len - 1] == '@'))
        {
            syslog(LOG_MAIL | LOG_INFO,
                   "srs_forward: <%s> not rewritten: Domain excluded by policy",
                   address);
            fputs("500 Domain excluded py policy\n", fp);
            fflush(fp);
            return;
        }
    }
    if (srs_reverse(srs, value, sizeof(value), address) == SRS_SUCCESS)
    {
        fprintf(fp, "500 Already rewritten\n");
        fflush(fp);
        syslog(LOG_MAIL | LOG_NOTICE,
               "srs_forward: <%s> not rewritten: Valid SRS address for <%s>",
               address, value);
        return;
    }
    result = srs_forward(srs, value, sizeof(value), address, domain);
    if (result == SRS_SUCCESS)
    {
        output = url_encode(outputbuf, sizeof(outputbuf), value);
        fprintf(fp, "200 %s\n", output);
        if (strcmp(address, value) != 0)
            syslog(LOG_MAIL | LOG_INFO, "srs_forward: <%s> rewritten as <%s>",
                   address, value);
    }
    else
    {
        fprintf(fp, "500 %s\n", srs_strerror(result));
        if (result != SRS_ENOTREWRITTEN)
            syslog(LOG_MAIL | LOG_INFO, "srs_forward: <%s> not rewritten: %s",
                   address, srs_strerror(result));
    }
    fflush(fp);
}