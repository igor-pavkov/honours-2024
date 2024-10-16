static void cmd_window_rsize(const char *data)
{
	char rsizestr[MAX_INT_STRLEN];
	int rsize;

	if (!is_numeric(data, 0)) return;
	rsize = atoi(data);

	rsize -= MAIN_WINDOW_TEXT_WIDTH(WINDOW_MAIN(active_win));
	if (rsize == 0) return;

	ltoa(rsizestr, rsize < 0 ? -rsize : rsize);
	if (rsize < 0)
		cmd_window_rshrink(rsizestr);
	else
		cmd_window_rgrow(rsizestr);
}