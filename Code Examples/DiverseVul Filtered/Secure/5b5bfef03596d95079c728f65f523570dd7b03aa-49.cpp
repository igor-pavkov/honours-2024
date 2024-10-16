static void cmd_window_move_down(void)
{
	MAIN_WINDOW_REC *rec;

	rec = mainwindows_find_lower_right(active_mainwin);
	if (rec != NULL)
		window_reparent(active_win, rec);
}