static void mainwindows_resize_two(GSList *grow_list,
				   GSList *shrink_list, int count)
{
	GSList *tmp;
	MAIN_WINDOW_REC *win;

	irssi_set_dirty();

	for (tmp = shrink_list; tmp != NULL; tmp = tmp->next) {
		win = tmp->data;
		mainwindow_resize(win, 0, -count);
		win->dirty = TRUE;
	}
	for (tmp = grow_list; tmp != NULL; tmp = tmp->next) {
		win = tmp->data;
		mainwindow_resize(win, 0, count);
		win->dirty = TRUE;
	}
}