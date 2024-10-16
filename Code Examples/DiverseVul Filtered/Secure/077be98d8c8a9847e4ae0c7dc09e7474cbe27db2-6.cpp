static void handle_reverse(srs_t *srs, FILE *fp, const char *address,
                           const char *domain __attribute__((unused)),
                           const char **excludes __attribute__((unused)))
{
    int result;
    char value[1024];
    char outputbuf[1024], *output;
    result = srs_reverse(srs, value, sizeof(value), address);
    if (result == SRS_SUCCESS)
    {
        output = url_encode(outputbuf, sizeof(outputbuf), value);
        fprintf(fp, "200 %s\n", output);
        syslog(LOG_MAIL | LOG_INFO, "srs_reverse: <%s> rewritten as <%s>",
               address, value);
    }
    else
    {
        fprintf(fp, "500 %s\n", srs_strerror(result));
        if (result != SRS_ENOTREWRITTEN && result != SRS_ENOTSRSADDRESS)
            syslog(LOG_MAIL | LOG_INFO, "srs_reverse: <%s> not rewritten: %s",
                   address, srs_strerror(result));
    }
    fflush(fp);
}