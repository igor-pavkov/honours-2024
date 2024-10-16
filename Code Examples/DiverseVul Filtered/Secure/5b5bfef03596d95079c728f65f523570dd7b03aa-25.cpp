static void mainwindows_rresize_line(int xdiff, MAIN_WINDOW_REC *win)
{
	int windows, i, extra_width, next_column, shrunk;
	int *widths;
	GSList *line, *tmp;

	line = mainwindows_get_line(win);
	windows = g_slist_length(line);
	widths = g_new0(int, windows);

	extra_width = screen_width - windows + 1;
	for (tmp = line, i = 0; tmp != NULL; tmp = tmp->next, i++) {
		MAIN_WINDOW_REC *rec = tmp->data;
		widths[i] = (MAIN_WINDOW_TEXT_WIDTH(rec) * (screen_width - windows + 1)) / (screen_width - xdiff - windows + 1);
		extra_width -= widths[i] + rec->statusbar_columns;
	}
	shrunk = FALSE;
	for (i = windows; extra_width < 0; i = i > 1 ? i - 1 : windows) {
		if (widths[i-1] > NEW_WINDOW_WIDTH || (i == 1 && !shrunk)) {
			widths[i-1]--;
			extra_width++;
			shrunk = i == 1;
		}
	}

	next_column = 0;

#define extra ( (i >= screen_width % windows && i < extra_width + (screen_width % windows)) \
		|| i + windows < extra_width + (screen_width % windows) ? 1 : 0 )

	for (tmp = line, i = 0; tmp != NULL; tmp = tmp->next, i++) {
		MAIN_WINDOW_REC *rec = tmp->data;
		rec->first_column = next_column;
		rec->last_column = rec->first_column + widths[i] + rec->statusbar_columns + extra - 1;
		next_column = rec->last_column + 2;
		mainwindow_resize(rec, widths[i] + rec->statusbar_columns + extra - rec->width, 0);
		rec->size_dirty = TRUE;
	}
#undef extra

	g_free(widths);
	g_slist_free(line);
}