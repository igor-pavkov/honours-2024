int mainwindows_reserve_lines(int top, int bottom)
{
	MAIN_WINDOW_REC *window;
	int ret;

	ret = -1;
	if (top != 0) {
		GSList *list, *tmp;
		g_return_val_if_fail(top > 0 || screen_reserved_top > top, -1);

		ret = screen_reserved_top;
		screen_reserved_top += top;

		list = mainwindows_get_line(mainwindows_find_lower(NULL));
		for (tmp = list; tmp != NULL; tmp = tmp->next) {
			window = tmp->data;
			window->first_line += top;
			mainwindow_resize(window, 0, -top);
		}
		g_slist_free(list);
	}

	if (bottom != 0) {
		GSList *list, *tmp;
		g_return_val_if_fail(bottom > 0 || screen_reserved_bottom > bottom, -1);

		ret = screen_reserved_bottom;
		screen_reserved_bottom += bottom;

		list = mainwindows_get_line(mainwindows_find_upper(NULL));
		for (tmp = list; tmp != NULL; tmp = tmp->next) {
			window = tmp->data;
			window->last_line -= bottom;
			mainwindow_resize(window, 0, -bottom);
		}
		g_slist_free(list);
	}

	return ret;
}