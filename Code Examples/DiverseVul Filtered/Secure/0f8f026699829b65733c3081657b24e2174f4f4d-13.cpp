static void cmd_starttls(int nntps __attribute__((unused)))
{
    /* XXX should never get here */
    fatal("cmd_starttls() called, but no OpenSSL", EC_SOFTWARE);
}