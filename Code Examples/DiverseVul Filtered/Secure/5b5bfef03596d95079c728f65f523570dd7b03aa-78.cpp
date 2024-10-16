static void cmd_window_left(const char *data, SERVER_REC *server, void *item)
{
	int refnum;

	refnum = window_refnum_left(active_win->refnum, TRUE);
	if (refnum != -1)
		window_set_active(window_find_refnum(refnum));
}