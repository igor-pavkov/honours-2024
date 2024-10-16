static void cmd_window_hide(const char *data)
{
	WINDOW_REC *window;

	if (mainwindows->next == NULL) {
		printformat_window(active_win, MSGLEVEL_CLIENTNOTICE,
				   TXT_CANT_HIDE_LAST);
		return;
	}

	if (*data == '\0')
		window = active_win;
	else if (is_numeric(data, 0)) {
		window = window_find_refnum(atoi(data));
		if (window == NULL) {
			printformat_window(active_win, MSGLEVEL_CLIENTERROR,
					   TXT_REFNUM_NOT_FOUND, data);
		}
	} else {
		window = window_find_item(active_win->active_server, data);
	}

	if (window == NULL || !is_window_visible(window))
		return;

	if (WINDOW_MAIN(window)->sticky_windows) {
		if (!settings_get_bool("autounstick_windows")) {
			printformat_window(active_win, MSGLEVEL_CLIENTERROR,
					   TXT_CANT_HIDE_STICKY_WINDOWS);
			return;
		}
	}

	mainwindow_destroy(WINDOW_MAIN(window));

	if (active_mainwin == NULL) {
		active_mainwin = WINDOW_MAIN(active_win);
		window_set_active(active_mainwin->active);
	}
}