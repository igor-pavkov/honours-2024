static int try_rgrow_left(MAIN_WINDOW_REC *window, int count)
{
	MAIN_WINDOW_REC *grow_win;

	grow_win = mainwindows_find_left(window, FALSE);
	if (grow_win != NULL) {
		grow_win->last_column += count;
		window->first_column += count;
		mainwindows_rresize_two(grow_win, window, count);
		return TRUE;
	}

	return FALSE;
}