static MAIN_WINDOW_REC *mainwindows_find_upper(MAIN_WINDOW_REC *window)
{
	int first_line;
	MAIN_WINDOW_REC *best;
	GSList *tmp;

	if (window != NULL)
		first_line = window->first_line;
	else
		first_line = screen_height;

	best = NULL;
	for (tmp = mainwindows; tmp != NULL; tmp = tmp->next) {
		MAIN_WINDOW_REC *rec = tmp->data;

		if (rec->last_line < first_line &&
		    (best == NULL || rec->last_line > best->last_line))
			best = rec;
	}

	return best;
}