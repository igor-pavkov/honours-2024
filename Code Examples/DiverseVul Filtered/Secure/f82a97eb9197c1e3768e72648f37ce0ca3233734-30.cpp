int is_valid_win32_path(const char *path)
{
	int preceding_space_or_period = 0, i = 0, periods = 0;

	if (!protect_ntfs)
		return 1;

	skip_dos_drive_prefix((char **)&path);

	for (;;) {
		char c = *(path++);
		switch (c) {
		case '\0':
		case '/': case '\\':
			/* cannot end in ` ` or `.`, except for `.` and `..` */
			if (preceding_space_or_period &&
			    (i != periods || periods > 2))
				return 0;
			if (!c)
				return 1;

			i = periods = preceding_space_or_period = 0;
			continue;
		case '.':
			periods++;
			/* fallthru */
		case ' ':
			preceding_space_or_period = 1;
			i++;
			continue;
		case ':': /* DOS drive prefix was already skipped */
		case '<': case '>': case '"': case '|': case '?': case '*':
			/* illegal character */
			return 0;
		default:
			if (c > '\0' && c < '\x20')
				/* illegal character */
				return 0;
		}
		preceding_space_or_period = 0;
		i++;
	}
}