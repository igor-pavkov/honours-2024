static void cmd_window_move_up(void)
{
	MAIN_WINDOW_REC *rec;

	rec = mainwindows_find_upper_left(active_mainwin);
	if (rec != NULL)
		window_reparent(active_win, rec);
}