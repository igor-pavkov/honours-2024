static void cmd_window_dright(void)
{
	MAIN_WINDOW_REC *rec;

	rec = mainwindows_find_right(active_mainwin, FALSE);
	if (rec == NULL)
		rec = mainwindows_find_right(active_mainwin, TRUE);
	if (rec != NULL)
		window_set_active(rec->active);
}