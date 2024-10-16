static void cmd_window_rgrow(const char *data)
{
	int count;

	count = *data == '\0' ? 1 : atoi(data);
	if (!mainwindow_rgrow(WINDOW_MAIN(active_win), count)) {
		printformat_window(active_win, MSGLEVEL_CLIENTNOTICE,
				   TXT_WINDOW_TOO_SMALL);
	}
}