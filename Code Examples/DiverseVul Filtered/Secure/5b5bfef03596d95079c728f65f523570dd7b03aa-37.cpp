static void cmd_window_shrink(const char *data)
{
	int count;

	count = *data == '\0' ? 1 : atoi(data);
	if (!mainwindow_shrink(WINDOW_MAIN(active_win), count, FALSE)) {
		printformat_window(active_win, MSGLEVEL_CLIENTNOTICE,
				   TXT_WINDOW_TOO_SMALL);
	}
}