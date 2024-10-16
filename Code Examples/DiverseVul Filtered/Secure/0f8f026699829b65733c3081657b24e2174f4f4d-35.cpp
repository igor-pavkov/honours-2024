static void cmd_newgroups(time_t tstamp __attribute__((unused)))
{
    prot_printf(nntp_out, "503 Can't determine NEWGROUPS at this time\r\n");
#if 0
    prot_printf(nntp_out, "231 List of new newsgroups follows:\r\n");

    /* Do search of annotations here. */

    prot_printf(nntp_out, ".\r\n");
#endif
}