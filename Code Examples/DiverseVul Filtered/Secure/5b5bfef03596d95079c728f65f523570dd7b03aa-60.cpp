static void cmd_window_move_dleft(void)
{
	MAIN_WINDOW_REC *rec;

	rec = mainwindows_find_left(active_mainwin, FALSE);
	if (rec == NULL)
		rec = mainwindows_find_left(active_mainwin, TRUE);
	if (rec != NULL)
		window_reparent(active_win, rec);
}