static void mainwindows_rresize_two(MAIN_WINDOW_REC *grow_win,
				    MAIN_WINDOW_REC *shrink_win, int count)
{
	irssi_set_dirty();

	mainwindow_resize(grow_win, count, 0);
	mainwindow_resize(shrink_win, -count, 0);
	grow_win->dirty = TRUE;
	shrink_win->dirty = TRUE;
}