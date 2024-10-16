void mainwindows_resize(int width, int height)
{
	int xdiff, ydiff;

	xdiff = width-screen_width;
	ydiff = height-screen_height;
	screen_width = width;
	screen_height = height;

	if (ydiff > 0) {
		/* algorithm: enlarge bottom window */
		MAIN_WINDOW_REC *rec;
		GSList *line, *tmp;
		line = mainwindows_get_line(mainwindows_find_upper(NULL));
		for (tmp = line; tmp != NULL; tmp = tmp->next) {
			rec = tmp->data;
			rec->last_line += ydiff;
			mainwindow_resize(rec, 0, ydiff);
		}
		g_slist_free(line);
	}

	if (xdiff > 0) {
		/* algorithm: distribute new space on each line */
		MAIN_WINDOW_REC *win;

		for (win = mainwindows_find_lower(NULL);
		     win != NULL;
		     win = mainwindows_find_lower(win)) {
			mainwindows_rresize_line(xdiff, win);
		}
	}

	if (xdiff < 0) {
		/* algorithm: shrink each window,
		   destroy windows on the right if no room */
		MAIN_WINDOW_REC *win;

		for (win = mainwindows_find_lower(NULL);
		     win != NULL;
		     win = mainwindows_find_lower(win)) {
			int max_windows, i, last_column;
			GSList *line, *tmp;

			line = mainwindows_get_line(win);
			max_windows = (screen_width + 1) / (NEW_WINDOW_WIDTH + 1);
			if (max_windows < 1)
				max_windows = 1;
			last_column = screen_width - 1;
			for (tmp = line, i = 0; tmp != NULL; tmp = tmp->next, i++) {
				MAIN_WINDOW_REC *rec = tmp->data;
				if (i >= max_windows)
					mainwindow_destroy_half(rec);
				else
					last_column = rec->last_column;
			}
			win = line->data;
			g_slist_free(line);

			mainwindows_rresize_line(screen_width - last_column + 1, win);
		}
	}

	if (ydiff < 0) {
		/* algorithm: shrink windows starting from bottom,
		   destroy windows starting from top if no room */
		mainwindows_resize_smaller(ydiff);
	}

	signal_emit("terminal resized", 0);

	irssi_redraw();
}