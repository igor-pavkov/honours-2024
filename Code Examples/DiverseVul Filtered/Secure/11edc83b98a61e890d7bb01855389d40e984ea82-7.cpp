static gboolean info_line(const gchar *line)
{
	int i=NETSCREEN_SPACES_ON_INFO_LINE;

	while (i-- > 0) {
		if (g_ascii_isspace(*line)) {
			line++;
			continue;
		} else {
			return FALSE;
		}
	}
	return TRUE;
}