static int try_shrink_upper(MAIN_WINDOW_REC *window, int count)
{
	MAIN_WINDOW_REC *shrink_win;

	shrink_win = mainwindows_find_upper(window);
	if (shrink_win != NULL) {
		int ok;
		GSList *shrink_list, *tmp;
		MAIN_WINDOW_REC *win;

		ok = TRUE;
		shrink_list = mainwindows_get_line(shrink_win);

		for (tmp = shrink_list; tmp != NULL; tmp = tmp->next) {
			win = tmp->data;
			if (MAIN_WINDOW_TEXT_HEIGHT(win)-count < WINDOW_MIN_SIZE) {
				ok = FALSE;
				break;
			}
		}
		if (ok) {
			GSList *grow_list;
			grow_list = mainwindows_get_line(window);
			for (tmp = grow_list; tmp != NULL; tmp = tmp->next) {
				win = tmp->data;
				win->first_line -= count;
			}
			for (tmp = shrink_list; tmp != NULL; tmp = tmp->next) {
				win = tmp->data;
				win->last_line -= count;
			}
			mainwindows_resize_two(grow_list, shrink_list, count);
			g_slist_free(grow_list);
		}
		g_slist_free(shrink_list);
		return ok;
	}

	return FALSE;
}