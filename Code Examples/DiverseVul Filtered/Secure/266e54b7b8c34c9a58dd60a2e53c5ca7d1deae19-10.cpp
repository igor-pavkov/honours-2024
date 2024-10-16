static int imap_parse_bodystructure_args(const struct imap_arg *args,
					 string_t *str, const char **error_r)
{
	const struct imap_arg *subargs;
	const struct imap_arg *list_args;
	const char *value, *content_type, *subtype;
	bool multipart, text, message_rfc822;
	int i;

	multipart = FALSE;
	while (args->type == IMAP_ARG_LIST) {
		str_append_c(str, '(');
		list_args = imap_arg_as_list(args);
		if (imap_parse_bodystructure_args(list_args, str, error_r) < 0)
			return -1;
		str_append_c(str, ')');

		multipart = TRUE;
		args++;
	}

	if (multipart) {
		/* next is subtype of Content-Type. rest is skipped. */
		str_append_c(str, ' ');
		if (!str_append_nstring(str, args)) {
			*error_r = "Invalid multipart content-type";
			return -1;
		}
		return 0;
	}

	/* "content type" "subtype" */
	if (!imap_arg_get_astring(&args[0], &content_type) ||
	    !imap_arg_get_astring(&args[1], &subtype)) {
		*error_r = "Invalid content-type";
		return -1;
	}

	if (!str_append_nstring(str, &args[0]))
		i_unreached();
	str_append_c(str, ' ');
	if (!str_append_nstring(str, &args[1]))
		i_unreached();

	text = strcasecmp(content_type, "text") == 0;
	message_rfc822 = strcasecmp(content_type, "message") == 0 &&
		strcasecmp(subtype, "rfc822") == 0;

	args += 2;

	/* ("content type param key" "value" ...) | NIL */
	if (imap_arg_get_list(args, &subargs)) {
		str_append(str, " (");
		while (!IMAP_ARG_IS_EOL(subargs)) {
			if (!str_append_nstring(str, &subargs[0])) {
				*error_r = "Invalid content param key";
				return -1;
			}
			str_append_c(str, ' ');
			if (!str_append_nstring(str, &subargs[1])) {
				*error_r = "Invalid content param value";
				return -1;
			}

			subargs += 2;
			if (IMAP_ARG_IS_EOL(subargs))
				break;
			str_append_c(str, ' ');
		}
		str_append(str, ")");
	} else if (args->type == IMAP_ARG_NIL) {
		str_append(str, " NIL");
	} else {
		*error_r = "list/NIL expected";
		return -1;
	}
	args++;

	/* "content id" "content description" "transfer encoding" size */
	for (i = 0; i < 4; i++, args++) {
		str_append_c(str, ' ');

		if (!str_append_nstring(str, args)) {
			*error_r = "nstring expected";
			return -1;
		}
	}

	if (text) {
		/* text/xxx - text lines */
		if (!imap_arg_get_atom(args, &value)) {
			*error_r = "Text lines expected";
			return -1;
		}

		str_append_c(str, ' ');
		str_append(str, value);
	} else if (message_rfc822) {
		/* message/rfc822 - envelope + bodystructure + text lines */
		str_append_c(str, ' ');

		if (!imap_arg_get_list(&args[0], &list_args)) {
			*error_r = "Envelope list expected";
			return -1;
		}
		str_append_c(str, '(');
		imap_write_envelope(list_args, str);
		str_append(str, ") (");

		if (!imap_arg_get_list(&args[1], &list_args)) {
			*error_r = "Child bodystructure list expected";
			return -1;
		}
		if (imap_parse_bodystructure_args(list_args, str, error_r) < 0)
			return -1;

		str_append(str, ") ");
		if (!imap_arg_get_atom(&args[2], &value)) {
			*error_r = "Text lines expected";
			return -1;
		}
		str_append(str, value);
	}
	return 0;
}