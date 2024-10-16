static MAIN_WINDOW_REC *mainwindows_find_right(MAIN_WINDOW_REC *window, int find_first)
{
	int first_line, last_line, last_column;
	MAIN_WINDOW_REC *best;
	GSList *tmp;

	if (window != NULL) {
		first_line = window->first_line;
		last_line = window->last_line;
		last_column = window->last_column;
	} else {
		first_line = last_line = last_column = -1;
	}

	if (find_first)
		last_column = -1;

	best = NULL;
	for (tmp = mainwindows; tmp != NULL; tmp = tmp->next) {
		MAIN_WINDOW_REC *rec = tmp->data;

		if (rec->first_line >= first_line &&
		    rec->last_line <= last_line &&
		    rec->first_column > last_column &&
		    (best == NULL || rec->first_column < best->first_column))
			best = rec;
	}

	return best;
}