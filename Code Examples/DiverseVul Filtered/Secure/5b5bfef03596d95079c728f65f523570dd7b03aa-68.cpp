static void mainwindows_add_space(MAIN_WINDOW_REC *destroy_win)
{
	MAIN_WINDOW_REC *rec;
	int size, rsize;

	if (destroy_win->last_line < destroy_win->first_line)
		return;

	if (destroy_win->last_column < destroy_win->first_column)
		return;

	rsize = destroy_win->last_column-destroy_win->first_column+1;
	rec = mainwindows_find_left(destroy_win, FALSE);
	if (rec != NULL) {
		rec->last_column = destroy_win->last_column;
		mainwindow_resize(rec, rsize+1, 0);
		return;
	}

	rec = mainwindows_find_right(destroy_win, FALSE);
	if (rec != NULL) {
		rec->first_column = destroy_win->first_column;
		mainwindow_resize(rec, rsize+1, 0);
		return;
	}

	size = destroy_win->last_line-destroy_win->first_line+1;

	rec = mainwindows_find_lower(destroy_win);
	if (rec != NULL) {
		GSList *tmp, *list;
		list = mainwindows_get_line(rec);

		for (tmp = list; tmp != NULL; tmp = tmp->next) {
			MAIN_WINDOW_REC *rec = tmp->data;
			rec->first_line = destroy_win->first_line;
			mainwindow_resize(rec, 0, size);
		}

		g_slist_free(list);
		return;
	}

	rec = mainwindows_find_upper(destroy_win);
	if (rec != NULL) {
		GSList *tmp, *list;
		list = mainwindows_get_line(rec);

		for (tmp = list; tmp != NULL; tmp = tmp->next) {
			MAIN_WINDOW_REC *rec = tmp->data;
			rec->last_line = destroy_win->last_line;
			mainwindow_resize(rec, 0, size);
		}

		g_slist_free(list);
		return;
	}
}