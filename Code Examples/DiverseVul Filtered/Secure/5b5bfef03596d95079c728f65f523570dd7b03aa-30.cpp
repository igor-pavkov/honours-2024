static void cmd_window_stick(const char *data)
{
        MAIN_WINDOW_REC *mainwin;
        WINDOW_REC *win;

        mainwin = active_mainwin;
        win = active_mainwin->active;

	if (is_numeric(data, ' ')) {
		/* ref# specified */
		win = window_find_refnum(atoi(data));
		if (win == NULL) {
			printformat_window(active_win, MSGLEVEL_CLIENTERROR,
					   TXT_REFNUM_NOT_FOUND, data);
			return;
		}

		while (*data != ' ' && *data != '\0') data++;
		while (*data == ' ') data++;
	}

	if (g_ascii_strncasecmp(data, "OF", 2) == 0 || i_toupper(*data) == 'N') {
		/* unset sticky */
		if (!WINDOW_GUI(win)->sticky) {
			printformat_window(win, MSGLEVEL_CLIENTERROR,
					   TXT_WINDOW_NOT_STICKY);
		} else {
                        gui_window_set_unsticky(win);
			printformat_window(win, MSGLEVEL_CLIENTNOTICE,
					   TXT_WINDOW_UNSET_STICKY);
		}
	} else {
		/* set sticky */
		window_reparent(win, mainwin);
                gui_window_set_sticky(win);

		printformat_window(active_win, MSGLEVEL_CLIENTNOTICE,
				   TXT_WINDOW_SET_STICKY);
	}
}