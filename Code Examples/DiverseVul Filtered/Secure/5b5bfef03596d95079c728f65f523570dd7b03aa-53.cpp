static void cmd_window_down(void)
{
	MAIN_WINDOW_REC *rec;

	rec = mainwindows_find_lower_right(active_mainwin);
	if (rec == NULL)
		rec = mainwindows_find_lower_right(NULL);
	if (rec != NULL)
		window_set_active(rec->active);
}