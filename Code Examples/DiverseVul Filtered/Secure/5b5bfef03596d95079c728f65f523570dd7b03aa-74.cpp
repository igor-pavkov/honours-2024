static void cmd_window_right(void)
{
	int refnum;

	refnum = window_refnum_right(active_win->refnum, TRUE);
	if (refnum != -1)
		window_set_active(window_find_refnum(refnum));
}