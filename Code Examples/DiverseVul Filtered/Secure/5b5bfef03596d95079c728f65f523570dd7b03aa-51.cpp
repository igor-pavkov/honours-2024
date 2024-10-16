GSList *mainwindows_get_line(MAIN_WINDOW_REC *rec)
{
	MAIN_WINDOW_REC *win;
	GSList *list;

	list = NULL;

	for (win = mainwindows_find_left(rec, FALSE);
	     win != NULL;
	     win = mainwindows_find_left(win, FALSE)) {
		list = g_slist_append(list, win);
	}

	if (rec != NULL)
		list = g_slist_append(list, rec);

	for (win = mainwindows_find_right(rec, FALSE);
	     win != NULL;
	     win = mainwindows_find_right(win, FALSE)) {
		list = g_slist_append(list, win);
	}

	return list;
}