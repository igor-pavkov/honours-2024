static int try_rgrow_right(MAIN_WINDOW_REC *window, int count)
{
	MAIN_WINDOW_REC *grow_win;

	grow_win = mainwindows_find_right(window, FALSE);
	if (grow_win != NULL) {
		grow_win->first_column -= count;
		window->last_column -= count;
		mainwindows_rresize_two(grow_win, window, count);
		return TRUE;
	}

	return FALSE;
}