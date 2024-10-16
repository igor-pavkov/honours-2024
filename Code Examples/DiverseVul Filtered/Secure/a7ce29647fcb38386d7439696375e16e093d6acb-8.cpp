static inline void sanitize_var_serial(char *name, bool colon) {
	r_return_if_fail (name);
	for (; *name; name++) {
		switch (*name) {
		case ':':
			if (colon) {
				break;
			}
		case '`':
		case '$':
		case '{':
		case '}':
		case '~':
		case '|':
		case '#':
		case '@':
		case '&':
		case '<':
		case '>':
		case ',':
			*name = '_';
			continue;
		}
	}
}