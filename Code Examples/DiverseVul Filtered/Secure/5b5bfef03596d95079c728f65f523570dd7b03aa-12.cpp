static void cmd_window_up(void)
{
	MAIN_WINDOW_REC *rec;

	rec = mainwindows_find_left_upper(active_mainwin);
	if (rec == NULL)
		rec = mainwindows_find_left_upper(NULL);
	if (rec != NULL)
		window_set_active(rec->active);
}