R_API int r_line_hist_sdb_up(RLine *line) {
	if (!line->sdbshell_hist_iter || !line->sdbshell_hist_iter->n) {
		return false;
	}
	line->sdbshell_hist_iter = line->sdbshell_hist_iter->n;
	strncpy (line->buffer.data, line->sdbshell_hist_iter->data, R_LINE_BUFSIZE - 1);
	line->buffer.index = line->buffer.length = strlen (line->buffer.data);
	return true;
}