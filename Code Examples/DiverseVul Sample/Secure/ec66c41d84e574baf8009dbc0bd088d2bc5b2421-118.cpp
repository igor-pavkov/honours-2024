winframe_remove(
    win_T	*win,
    int		*dirp UNUSED,	/* set to 'v' or 'h' for direction if 'ea' */
    tabpage_T	*tp)		/* tab page "win" is in, NULL for current */
{
    frame_T	*frp, *frp2, *frp3;
    frame_T	*frp_close = win->w_frame;
    win_T	*wp;

    /*
     * If there is only one window there is nothing to remove.
     */
    if (tp == NULL ? ONE_WINDOW : tp->tp_firstwin == tp->tp_lastwin)
	return NULL;

    /*
     * Remove the window from its frame.
     */
    frp2 = win_altframe(win, tp);
    wp = frame2win(frp2);

    /* Remove this frame from the list of frames. */
    frame_remove(frp_close);

    if (frp_close->fr_parent->fr_layout == FR_COL)
    {
	/* When 'winfixheight' is set, try to find another frame in the column
	 * (as close to the closed frame as possible) to distribute the height
	 * to. */
	if (frp2->fr_win != NULL && frp2->fr_win->w_p_wfh)
	{
	    frp = frp_close->fr_prev;
	    frp3 = frp_close->fr_next;
	    while (frp != NULL || frp3 != NULL)
	    {
		if (frp != NULL)
		{
		    if (!frame_fixed_height(frp))
		    {
			frp2 = frp;
			wp = frame2win(frp2);
			break;
		    }
		    frp = frp->fr_prev;
		}
		if (frp3 != NULL)
		{
		    if (frp3->fr_win != NULL && !frp3->fr_win->w_p_wfh)
		    {
			frp2 = frp3;
			wp = frp3->fr_win;
			break;
		    }
		    frp3 = frp3->fr_next;
		}
	    }
	}
	frame_new_height(frp2, frp2->fr_height + frp_close->fr_height,
			    frp2 == frp_close->fr_next ? TRUE : FALSE, FALSE);
	*dirp = 'v';
    }
    else
    {
	/* When 'winfixwidth' is set, try to find another frame in the column
	 * (as close to the closed frame as possible) to distribute the width
	 * to. */
	if (frp2->fr_win != NULL && frp2->fr_win->w_p_wfw)
	{
	    frp = frp_close->fr_prev;
	    frp3 = frp_close->fr_next;
	    while (frp != NULL || frp3 != NULL)
	    {
		if (frp != NULL)
		{
		    if (!frame_fixed_width(frp))
		    {
			frp2 = frp;
			wp = frame2win(frp2);
			break;
		    }
		    frp = frp->fr_prev;
		}
		if (frp3 != NULL)
		{
		    if (frp3->fr_win != NULL && !frp3->fr_win->w_p_wfw)
		    {
			frp2 = frp3;
			wp = frp3->fr_win;
			break;
		    }
		    frp3 = frp3->fr_next;
		}
	    }
	}
	frame_new_width(frp2, frp2->fr_width + frp_close->fr_width,
			    frp2 == frp_close->fr_next ? TRUE : FALSE, FALSE);
	*dirp = 'h';
    }

    /* If rows/columns go to a window below/right its positions need to be
     * updated.  Can only be done after the sizes have been updated. */
    if (frp2 == frp_close->fr_next)
    {
	int row = win->w_winrow;
	int col = win->w_wincol;

	frame_comp_pos(frp2, &row, &col);
    }

    if (frp2->fr_next == NULL && frp2->fr_prev == NULL)
    {
	/* There is no other frame in this list, move its info to the parent
	 * and remove it. */
	frp2->fr_parent->fr_layout = frp2->fr_layout;
	frp2->fr_parent->fr_child = frp2->fr_child;
	FOR_ALL_FRAMES(frp, frp2->fr_child)
	    frp->fr_parent = frp2->fr_parent;
	frp2->fr_parent->fr_win = frp2->fr_win;
	if (frp2->fr_win != NULL)
	    frp2->fr_win->w_frame = frp2->fr_parent;
	frp = frp2->fr_parent;
	if (topframe->fr_child == frp2)
	    topframe->fr_child = frp;
	vim_free(frp2);

	frp2 = frp->fr_parent;
	if (frp2 != NULL && frp2->fr_layout == frp->fr_layout)
	{
	    /* The frame above the parent has the same layout, have to merge
	     * the frames into this list. */
	    if (frp2->fr_child == frp)
		frp2->fr_child = frp->fr_child;
	    frp->fr_child->fr_prev = frp->fr_prev;
	    if (frp->fr_prev != NULL)
		frp->fr_prev->fr_next = frp->fr_child;
	    for (frp3 = frp->fr_child; ; frp3 = frp3->fr_next)
	    {
		frp3->fr_parent = frp2;
		if (frp3->fr_next == NULL)
		{
		    frp3->fr_next = frp->fr_next;
		    if (frp->fr_next != NULL)
			frp->fr_next->fr_prev = frp3;
		    break;
		}
	    }
	    if (topframe->fr_child == frp)
		topframe->fr_child = frp2;
	    vim_free(frp);
	}
    }

    return wp;
}