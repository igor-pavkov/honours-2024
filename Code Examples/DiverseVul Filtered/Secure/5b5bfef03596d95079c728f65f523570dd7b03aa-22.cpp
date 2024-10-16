static void cmd_window_balance(void)
{
	GSList *sorted, *stmp, *line, *ltmp;
	int avail_size, unit_size, bigger_units;
	int windows, last_line, old_size;
	MAIN_WINDOW_REC *win;

	windows = g_slist_length(mainwindows);
	if (windows == 1) return;

	sorted = NULL;
	windows = 0;
	for (win = mainwindows_find_lower(NULL);
	     win != NULL;
	     win = mainwindows_find_lower(win)) {
		windows++;
		sorted = g_slist_append(sorted, win);
	}

	avail_size = term_height - screen_reserved_top-screen_reserved_bottom;
	unit_size = avail_size/windows;
	bigger_units = avail_size%windows;

	last_line = screen_reserved_top;
	for (stmp = sorted; stmp != NULL; stmp = stmp->next) {
		win = stmp->data;
		line = mainwindows_get_line(win);

		for (ltmp = line; ltmp != NULL; ltmp = ltmp->next) {
			MAIN_WINDOW_REC *rec = ltmp->data;
			old_size = rec->height;
			rec->first_line = last_line;
			rec->last_line = rec->first_line + unit_size-1;

			if (bigger_units > 0) {
				rec->last_line++;
			}

			rec->height = rec->last_line-rec->first_line+1;

			mainwindow_resize(rec, 0, rec->height-old_size);
		}
		if (line != NULL && bigger_units > 0) {
			bigger_units--;
		}
		last_line = win->last_line+1;

		g_slist_free(line);
	}
	g_slist_free(sorted);

	mainwindows_redraw();
}