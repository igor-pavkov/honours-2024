static gboolean netscreen_check_file_type(wtap *wth, int *err, gchar **err_info)
{
	char	buf[NETSCREEN_LINE_LENGTH];
	guint	reclen, line;

	buf[NETSCREEN_LINE_LENGTH-1] = '\0';

	for (line = 0; line < NETSCREEN_HEADER_LINES_TO_CHECK; line++) {
		if (file_gets(buf, NETSCREEN_LINE_LENGTH, wth->fh) == NULL) {
			/* EOF or error. */
			*err = file_error(wth->fh, err_info);
			return FALSE;
		}

		reclen = (guint) strlen(buf);
		if (reclen < strlen(NETSCREEN_HDR_MAGIC_STR1) ||
			reclen < strlen(NETSCREEN_HDR_MAGIC_STR2)) {
			continue;
		}

		if (strstr(buf, NETSCREEN_HDR_MAGIC_STR1) ||
		    strstr(buf, NETSCREEN_HDR_MAGIC_STR2)) {
			return TRUE;
		}
	}
	*err = 0;
	return FALSE;
}