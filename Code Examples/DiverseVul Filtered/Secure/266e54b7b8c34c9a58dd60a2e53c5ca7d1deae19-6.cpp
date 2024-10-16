static bool str_append_nstring(string_t *str, const struct imap_arg *arg)
{
	const char *cstr;

	if (!imap_arg_get_nstring(arg, &cstr))
		return FALSE;

	switch (arg->type) {
	case IMAP_ARG_NIL:
		str_append(str, "NIL");
		break;
	case IMAP_ARG_ATOM:
		str_append(str, cstr);
		break;
	case IMAP_ARG_STRING:
		str_append_c(str, '"');
		/* NOTE: we're parsing with no-unescape flag,
		   so don't double-escape it here */
		str_append(str, cstr);
		str_append_c(str, '"');
		break;
	case IMAP_ARG_LITERAL: {
		str_printfa(str, "{%zu}\r\n", strlen(cstr));
		str_append(str, cstr);
		break;
	}
	default:
		i_unreached();
		return FALSE;
	}
	return TRUE;
}