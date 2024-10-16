static void cmd_window_ddown(void)
{
	MAIN_WINDOW_REC *rec;

	rec = mainwindows_find_lower(active_mainwin);
	if (rec == NULL)
		rec = mainwindows_find_lower(NULL);
	if (rec != NULL)
		window_set_active(rec->active);
}