static void cmd_newnews(char *wild, time_t tstamp)
{
    struct newrock nrock;

    nrock.tstamp = tstamp;
    nrock.wild = split_wildmats(wild);

    prot_printf(nntp_out, "230 List of new articles follows:\r\n");

    newnews_cb(NULL, NULL, 0, 0, NULL);
    duplicate_find("", &newnews_cb, &nrock);

    prot_printf(nntp_out, ".\r\n");

    free_wildmats(nrock.wild);
}