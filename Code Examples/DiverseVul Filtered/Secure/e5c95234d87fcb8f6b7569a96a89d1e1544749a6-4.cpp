static void phar_do_404(phar_archive_data *phar, char *fname, int fname_len, char *f404, int f404_len, char *entry, size_t entry_len) /* {{{ */
{
	sapi_header_line ctr = {0};
	phar_entry_info	*info;

	if (phar && f404_len) {
		info = phar_get_entry_info(phar, f404, f404_len, NULL, 1);

		if (info) {
			phar_file_action(phar, info, "text/html", PHAR_MIME_PHP, f404, f404_len, fname, NULL, NULL, 0);
			return;
		}
	}

	ctr.response_code = 404;
	ctr.line_len = sizeof("HTTP/1.0 404 Not Found")-1;
	ctr.line = "HTTP/1.0 404 Not Found";
	sapi_header_op(SAPI_HEADER_REPLACE, &ctr);
	sapi_send_headers();
	PHPWRITE("<html>\n <head>\n  <title>File Not Found</title>\n </head>\n <body>\n  <h1>404 - File ", sizeof("<html>\n <head>\n  <title>File Not Found</title>\n </head>\n <body>\n  <h1>404 - File ") - 1);
	PHPWRITE("Not Found</h1>\n </body>\n</html>",  sizeof("Not Found</h1>\n </body>\n</html>") - 1);
}