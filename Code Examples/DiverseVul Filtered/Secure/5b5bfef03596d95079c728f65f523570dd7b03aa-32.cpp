static MAIN_WINDOW_REC *mainwindows_find_left(MAIN_WINDOW_REC *window, int find_last)
{
	int first_line, last_line, first_column;
	MAIN_WINDOW_REC *best;
	GSList *tmp;

	if (window != NULL) {
		first_line = window->first_line;
		last_line = window->last_line;
		first_column = window->first_column;
	} else {
		first_line = last_line = screen_height;
		first_column = screen_width;
	}

	if (find_last)
		first_column = screen_width;

	best = NULL;
	for (tmp = mainwindows; tmp != NULL; tmp = tmp->next) {
		MAIN_WINDOW_REC *rec = tmp->data;

		if (rec->first_line >= first_line &&
		    rec->last_line <= last_line &&
		    rec->last_column < first_column &&
		    (best == NULL || rec->last_column > best->last_column))
			best = rec;
	}

	return best;
}