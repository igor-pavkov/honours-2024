params_write(const struct message_part_param *params,
	unsigned int params_count, string_t *str,
	bool default_charset)
{
	unsigned int i;
	bool seen_charset;

	if (!default_charset && params_count == 0) {
		str_append(str, "NIL");
		return;
	}
	str_append_c(str, '(');

	seen_charset = FALSE;
	for (i = 0; i < params_count; i++) {
		if (i > 0)
			str_append_c(str, ' ');
		if (default_charset &&
			strcasecmp(params[i].name, "charset") == 0)
			seen_charset = TRUE;
		imap_append_string(str, params[i].name);
		str_append_c(str, ' ');
		imap_append_string(str, params[i].value);
	}
	if (default_charset && !seen_charset) {
		if (i > 0)
			str_append_c(str, ' ');
		str_append(str, "\"charset\" "
			"\""MESSAGE_PART_DEFAULT_CHARSET"\"");
	}
	str_append_c(str, ')');
}