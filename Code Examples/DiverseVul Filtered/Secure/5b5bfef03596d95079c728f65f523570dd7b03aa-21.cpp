static MAIN_WINDOW_REC *mainwindows_find_lower(MAIN_WINDOW_REC *window)
{
	int last_line;
	MAIN_WINDOW_REC *best;
	GSList *tmp;

	if (window != NULL)
		last_line = window->last_line;
	else
		last_line = -1;

	best = NULL;
	for (tmp = mainwindows; tmp != NULL; tmp = tmp->next) {
		MAIN_WINDOW_REC *rec = tmp->data;

		if (rec->first_line > last_line &&
		    (best == NULL || rec->first_line < best->first_line))
			best = rec;
	}

	return best;
}