static void cmd_window_move_right(void)
{
	int refnum;

	refnum = window_refnum_right(active_win->refnum, TRUE);
	if (refnum != -1)
		window_set_refnum(active_win, refnum);
}