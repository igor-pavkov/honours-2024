static int window_refnum_right(int refnum, int wrap)
{
        MAIN_WINDOW_REC *find_sticky;
	WINDOW_REC *window;

	window = window_find_refnum(refnum);
	g_return_val_if_fail(window != NULL, -1);

	find_sticky = WINDOW_MAIN(window)->sticky_windows ?
		WINDOW_MAIN(window) : NULL;

	do {
		refnum = window_refnum_next(refnum, wrap);
		if (refnum < 0)
			break;

		window = window_find_refnum(refnum);
	} while (!WINDOW_STICKY_MATCH(window, find_sticky));

        return refnum;
}