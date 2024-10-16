last_status_rec(frame_T *fr, int statusline)
{
    frame_T	*fp;
    win_T	*wp;

    if (fr->fr_layout == FR_LEAF)
    {
	wp = fr->fr_win;
	if (wp->w_status_height != 0 && !statusline)
	{
	    /* remove status line */
	    win_new_height(wp, wp->w_height + 1);
	    wp->w_status_height = 0;
	    comp_col();
	}
	else if (wp->w_status_height == 0 && statusline)
	{
	    /* Find a frame to take a line from. */
	    fp = fr;
	    while (fp->fr_height <= frame_minheight(fp, NULL))
	    {
		if (fp == topframe)
		{
		    emsg(_(e_noroom));
		    return;
		}
		/* In a column of frames: go to frame above.  If already at
		 * the top or in a row of frames: go to parent. */
		if (fp->fr_parent->fr_layout == FR_COL && fp->fr_prev != NULL)
		    fp = fp->fr_prev;
		else
		    fp = fp->fr_parent;
	    }
	    wp->w_status_height = 1;
	    if (fp != fr)
	    {
		frame_new_height(fp, fp->fr_height - 1, FALSE, FALSE);
		frame_fix_height(wp);
		(void)win_comp_pos();
	    }
	    else
		win_new_height(wp, wp->w_height - 1);
	    comp_col();
	    redraw_all_later(SOME_VALID);
	}
    }
    else if (fr->fr_layout == FR_ROW)
    {
	/* vertically split windows, set status line for each one */
	FOR_ALL_FRAMES(fp, fr->fr_child)
	    last_status_rec(fp, statusline);
    }
    else
    {
	/* horizontally split window, set status line for last one */
	for (fp = fr->fr_child; fp->fr_next != NULL; fp = fp->fr_next)
	    ;
	last_status_rec(fp, statusline);
    }
}