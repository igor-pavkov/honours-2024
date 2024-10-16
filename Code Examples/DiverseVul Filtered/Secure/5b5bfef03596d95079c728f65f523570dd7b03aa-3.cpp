static void cmd_window_grow(const char *data)
{
	MAIN_WINDOW_REC *window;
	int count;

	count = *data == '\0' ? 1 : atoi(data);
	window = WINDOW_MAIN(active_win);

	if (!mainwindow_grow(window, count, FALSE)) {
		printformat_window(active_win, MSGLEVEL_CLIENTNOTICE,
				   TXT_WINDOW_TOO_SMALL);
	}
}