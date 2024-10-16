static int gen_request(
	git_buf *buf,
	http_stream *s,
	size_t content_length)
{
	http_subtransport *t = OWNING_SUBTRANSPORT(s);
	const char *path = t->connection_data.path ? t->connection_data.path : "/";
	size_t i;

	git_buf_printf(buf, "%s %s%s HTTP/1.1\r\n", s->verb, path, s->service_url);

	git_buf_printf(buf, "User-Agent: git/1.0 (%s)\r\n", user_agent());
	git_buf_printf(buf, "Host: %s\r\n", t->connection_data.host);

	if (s->chunked || content_length > 0) {
		git_buf_printf(buf, "Accept: application/x-git-%s-result\r\n", s->service);
		git_buf_printf(buf, "Content-Type: application/x-git-%s-request\r\n", s->service);

		if (s->chunked)
			git_buf_puts(buf, "Transfer-Encoding: chunked\r\n");
		else
			git_buf_printf(buf, "Content-Length: %"PRIuZ "\r\n", content_length);
	} else
		git_buf_puts(buf, "Accept: */*\r\n");

	for (i = 0; i < t->owner->custom_headers.count; i++) {
		if (t->owner->custom_headers.strings[i])
			git_buf_printf(buf, "%s\r\n", t->owner->custom_headers.strings[i]);
	}

	/* Apply credentials to the request */
	if (apply_credentials(buf, t) < 0)
		return -1;

	git_buf_puts(buf, "\r\n");

	if (git_buf_oom(buf))
		return -1;

	return 0;
}