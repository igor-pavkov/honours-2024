void fatal(const char* s, int code)
{
    static int recurse_code = 0;

    if (recurse_code) {
	/* We were called recursively. Just give up */
	proc_cleanup();
	exit(recurse_code);
    }
    recurse_code = code;
    if (nntp_out) {
	prot_printf(nntp_out, "400 Fatal error: %s\r\n", s);
	prot_flush(nntp_out);
    }
    if (stage) append_removestage(stage);
    syslog(LOG_ERR, "Fatal error: %s", s);
    shut_down(code);
}