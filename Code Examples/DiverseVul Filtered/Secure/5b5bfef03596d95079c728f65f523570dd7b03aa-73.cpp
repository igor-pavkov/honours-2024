static void cmd_window_rbalance(void)
{
	GSList *line, *ltmp;
	int avail_width, unit_width, bigger_units;
	int windows, last_column, old_width;
	MAIN_WINDOW_REC *win;

	line = mainwindows_get_line(WINDOW_MAIN(active_win));
	windows = g_slist_length(line);
	if (windows == 1) {
		g_slist_free(line);
		return;
	}

	avail_width = term_width - screen_reserved_left-screen_reserved_right - windows + 1;
	unit_width = avail_width/windows;
	bigger_units = avail_width%windows;

	last_column = screen_reserved_left;
	for (ltmp = line; ltmp != NULL; ltmp = ltmp->next) {
		win = ltmp->data;
		old_width = win->width;
		win->first_column = last_column;
		win->last_column = win->first_column + unit_width-1;

		if (bigger_units > 0) {
			win->last_column++;
			bigger_units--;
		}

		mainwindow_resize(win, win->last_column - win->first_column + 1 - old_width, 0);
		last_column = win->last_column+2;
	}
	g_slist_free(line);

	mainwindows_redraw();
}