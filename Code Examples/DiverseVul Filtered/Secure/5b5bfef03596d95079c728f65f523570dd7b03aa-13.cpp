static int try_grow_lower(MAIN_WINDOW_REC *window, int count)
{
	MAIN_WINDOW_REC *grow_win;

	grow_win = mainwindows_find_lower(window);
	if (grow_win != NULL) {
		MAIN_WINDOW_REC *win;
		GSList *grow_list, *shrink_list, *tmp;
		grow_list = mainwindows_get_line(grow_win);
		shrink_list = mainwindows_get_line(window);
		for (tmp = grow_list; tmp != NULL; tmp = tmp->next) {
			win = tmp->data;
			win->first_line -= count;
		}
		for (tmp = shrink_list; tmp != NULL; tmp = tmp->next) {
			win = tmp->data;
			win->last_line -= count;
		}
		mainwindows_resize_two(grow_list, shrink_list, count);
		g_slist_free(shrink_list);
		g_slist_free(grow_list);
	}

	return grow_win != NULL;
}