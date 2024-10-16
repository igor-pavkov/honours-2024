static char fgets_trim_sanitize(FILE *fp, const char *fmt)
{
	char c;
	char *buf_ptr;

	set_alarm();
	if (fgets(G.wget_buf, sizeof(G.wget_buf), fp) == NULL)
		bb_simple_perror_msg_and_die("error getting response");
	clear_alarm();

	buf_ptr = strchrnul(G.wget_buf, '\n');
	c = *buf_ptr;
#if 1
	/* Disallow any control chars: trim at first char < 0x20 */
	sanitize_string(G.wget_buf);
#else
	*buf_ptr = '\0';
	buf_ptr = strchrnul(G.wget_buf, '\r');
	*buf_ptr = '\0';
#endif

	log_io("< %s", G.wget_buf);

	if (fmt && (option_mask32 & WGET_OPT_SERVER_RESPONSE))
		fprintf(stderr, fmt, G.wget_buf);

	return c;
}