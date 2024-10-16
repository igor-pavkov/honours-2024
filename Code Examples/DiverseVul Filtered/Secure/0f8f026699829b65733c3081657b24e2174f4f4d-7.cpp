void usage(void)
{
    prot_printf(nntp_out, "503 usage: nntpd [-C <alt_config>] [-s]\r\n");
    prot_flush(nntp_out);
    exit(EC_USAGE);
}