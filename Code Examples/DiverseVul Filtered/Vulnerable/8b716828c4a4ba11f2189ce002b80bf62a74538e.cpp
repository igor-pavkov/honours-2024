static const char *escape_identifier(const char *identifier)
{
	size_t pos = strcspn(identifier, "./%");
	if (pos < strlen(identifier)) {
		/* sanitize identifier, cannot allow dots or / in it, so we
		  encode them */
		string_t *new_id = t_str_new(strlen(identifier));
		/* put initial data */
		str_append_data(new_id, identifier, pos);

		for (const char *c = identifier+pos; *c != '\0'; c++) {
			switch (*c) {
			case '.':
				str_append(new_id, "%2e");
				break;
			case '/':
				str_append(new_id, "%2f");
				break;
			case '%':
				str_append(new_id, "%25");
				break;
			default:
				str_append_c(new_id, *c);
				break;
			}
		}
		return str_c(new_id);
	}
	return identifier;
}