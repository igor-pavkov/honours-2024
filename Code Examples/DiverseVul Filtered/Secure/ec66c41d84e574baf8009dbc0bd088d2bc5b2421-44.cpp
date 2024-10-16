win_split_ins(
    int		size,
    int		flags,
    win_T	*new_wp,
    int		dir)
{
    win_T	*wp = new_wp;
    win_T	*oldwin;
    int		new_size = size;
    int		i;
    int		need_status = 0;
    int		do_equal = FALSE;
    int		needed;
    int		available;
    int		oldwin_height = 0;
    int		layout;
    frame_T	*frp, *curfrp, *frp2, *prevfrp;
    int		before;
    int		minheight;
    int		wmh1;
    int		did_set_fraction = FALSE;

    if (flags & WSP_TOP)
	oldwin = firstwin;
    else if (flags & WSP_BOT)
	oldwin = lastwin;
    else
	oldwin = curwin;

    /* add a status line when p_ls == 1 and splitting the first window */
    if (ONE_WINDOW && p_ls == 1 && oldwin->w_status_height == 0)
    {
	if (VISIBLE_HEIGHT(oldwin) <= p_wmh && new_wp == NULL)
	{
	    emsg(_(e_noroom));
	    return FAIL;
	}
	need_status = STATUS_HEIGHT;
    }

#ifdef FEAT_GUI
    /* May be needed for the scrollbars that are going to change. */
    if (gui.in_use)
	out_flush();
#endif

    if (flags & WSP_VERT)
    {
	int	wmw1;
	int	minwidth;

	layout = FR_ROW;

	/*
	 * Check if we are able to split the current window and compute its
	 * width.
	 */
	/* Current window requires at least 1 space. */
	wmw1 = (p_wmw == 0 ? 1 : p_wmw);
	needed = wmw1 + 1;
	if (flags & WSP_ROOM)
	    needed += p_wiw - wmw1;
	if (flags & (WSP_BOT | WSP_TOP))
	{
	    minwidth = frame_minwidth(topframe, NOWIN);
	    available = topframe->fr_width;
	    needed += minwidth;
	}
	else if (p_ea)
	{
	    minwidth = frame_minwidth(oldwin->w_frame, NOWIN);
	    prevfrp = oldwin->w_frame;
	    for (frp = oldwin->w_frame->fr_parent; frp != NULL;
							frp = frp->fr_parent)
	    {
		if (frp->fr_layout == FR_ROW)
		    FOR_ALL_FRAMES(frp2, frp->fr_child)
			if (frp2 != prevfrp)
			    minwidth += frame_minwidth(frp2, NOWIN);
		prevfrp = frp;
	    }
	    available = topframe->fr_width;
	    needed += minwidth;
	}
	else
	{
	    minwidth = frame_minwidth(oldwin->w_frame, NOWIN);
	    available = oldwin->w_frame->fr_width;
	    needed += minwidth;
	}
	if (available < needed && new_wp == NULL)
	{
	    emsg(_(e_noroom));
	    return FAIL;
	}
	if (new_size == 0)
	    new_size = oldwin->w_width / 2;
	if (new_size > available - minwidth - 1)
	    new_size = available - minwidth - 1;
	if (new_size < wmw1)
	    new_size = wmw1;

	/* if it doesn't fit in the current window, need win_equal() */
	if (oldwin->w_width - new_size - 1 < p_wmw)
	    do_equal = TRUE;

	/* We don't like to take lines for the new window from a
	 * 'winfixwidth' window.  Take them from a window to the left or right
	 * instead, if possible. Add one for the separator. */
	if (oldwin->w_p_wfw)
	    win_setwidth_win(oldwin->w_width + new_size + 1, oldwin);

	/* Only make all windows the same width if one of them (except oldwin)
	 * is wider than one of the split windows. */
	if (!do_equal && p_ea && size == 0 && *p_ead != 'v'
					 && oldwin->w_frame->fr_parent != NULL)
	{
	    frp = oldwin->w_frame->fr_parent->fr_child;
	    while (frp != NULL)
	    {
		if (frp->fr_win != oldwin && frp->fr_win != NULL
			&& (frp->fr_win->w_width > new_size
			    || frp->fr_win->w_width > oldwin->w_width
							      - new_size - 1))
		{
		    do_equal = TRUE;
		    break;
		}
		frp = frp->fr_next;
	    }
	}
    }
    else
    {
	layout = FR_COL;

	/*
	 * Check if we are able to split the current window and compute its
	 * height.
	 */
	/* Current window requires at least 1 space. */
	wmh1 = (p_wmh == 0 ? 1 : p_wmh) + WINBAR_HEIGHT(curwin);
	needed = wmh1 + STATUS_HEIGHT;
	if (flags & WSP_ROOM)
	    needed += p_wh - wmh1;
	if (flags & (WSP_BOT | WSP_TOP))
	{
	    minheight = frame_minheight(topframe, NOWIN) + need_status;
	    available = topframe->fr_height;
	    needed += minheight;
	}
	else if (p_ea)
	{
	    minheight = frame_minheight(oldwin->w_frame, NOWIN) + need_status;
	    prevfrp = oldwin->w_frame;
	    for (frp = oldwin->w_frame->fr_parent; frp != NULL;
							frp = frp->fr_parent)
	    {
		if (frp->fr_layout == FR_COL)
		    FOR_ALL_FRAMES(frp2, frp->fr_child)
			if (frp2 != prevfrp)
			    minheight += frame_minheight(frp2, NOWIN);
		prevfrp = frp;
	    }
	    available = topframe->fr_height;
	    needed += minheight;
	}
	else
	{
	    minheight = frame_minheight(oldwin->w_frame, NOWIN) + need_status;
	    available = oldwin->w_frame->fr_height;
	    needed += minheight;
	}
	if (available < needed && new_wp == NULL)
	{
	    emsg(_(e_noroom));
	    return FAIL;
	}
	oldwin_height = oldwin->w_height;
	if (need_status)
	{
	    oldwin->w_status_height = STATUS_HEIGHT;
	    oldwin_height -= STATUS_HEIGHT;
	}
	if (new_size == 0)
	    new_size = oldwin_height / 2;
	if (new_size > available - minheight - STATUS_HEIGHT)
	    new_size = available - minheight - STATUS_HEIGHT;
	if (new_size < wmh1)
	    new_size = wmh1;

	/* if it doesn't fit in the current window, need win_equal() */
	if (oldwin_height - new_size - STATUS_HEIGHT < p_wmh)
	    do_equal = TRUE;

	/* We don't like to take lines for the new window from a
	 * 'winfixheight' window.  Take them from a window above or below
	 * instead, if possible. */
	if (oldwin->w_p_wfh)
	{
	    /* Set w_fraction now so that the cursor keeps the same relative
	     * vertical position using the old height. */
	    set_fraction(oldwin);
	    did_set_fraction = TRUE;

	    win_setheight_win(oldwin->w_height + new_size + STATUS_HEIGHT,
								      oldwin);
	    oldwin_height = oldwin->w_height;
	    if (need_status)
		oldwin_height -= STATUS_HEIGHT;
	}

	/* Only make all windows the same height if one of them (except oldwin)
	 * is higher than one of the split windows. */
	if (!do_equal && p_ea && size == 0 && *p_ead != 'h'
	   && oldwin->w_frame->fr_parent != NULL)
	{
	    frp = oldwin->w_frame->fr_parent->fr_child;
	    while (frp != NULL)
	    {
		if (frp->fr_win != oldwin && frp->fr_win != NULL
			&& (frp->fr_win->w_height > new_size
			    || frp->fr_win->w_height > oldwin_height - new_size
							      - STATUS_HEIGHT))
		{
		    do_equal = TRUE;
		    break;
		}
		frp = frp->fr_next;
	    }
	}
    }

    /*
     * allocate new window structure and link it in the window list
     */
    if ((flags & WSP_TOP) == 0
	    && ((flags & WSP_BOT)
		|| (flags & WSP_BELOW)
		|| (!(flags & WSP_ABOVE)
		    && ( (flags & WSP_VERT) ? p_spr : p_sb))))
    {
	/* new window below/right of current one */
	if (new_wp == NULL)
	    wp = win_alloc(oldwin, FALSE);
	else
	    win_append(oldwin, wp);
    }
    else
    {
	if (new_wp == NULL)
	    wp = win_alloc(oldwin->w_prev, FALSE);
	else
	    win_append(oldwin->w_prev, wp);
    }

    if (new_wp == NULL)
    {
	if (wp == NULL)
	    return FAIL;

	new_frame(wp);
	if (wp->w_frame == NULL)
	{
	    win_free(wp, NULL);
	    return FAIL;
	}

	/* make the contents of the new window the same as the current one */
	win_init(wp, curwin, flags);
    }

    /*
     * Reorganise the tree of frames to insert the new window.
     */
    if (flags & (WSP_TOP | WSP_BOT))
    {
	if ((topframe->fr_layout == FR_COL && (flags & WSP_VERT) == 0)
	    || (topframe->fr_layout == FR_ROW && (flags & WSP_VERT) != 0))
	{
	    curfrp = topframe->fr_child;
	    if (flags & WSP_BOT)
		while (curfrp->fr_next != NULL)
		    curfrp = curfrp->fr_next;
	}
	else
	    curfrp = topframe;
	before = (flags & WSP_TOP);
    }
    else
    {
	curfrp = oldwin->w_frame;
	if (flags & WSP_BELOW)
	    before = FALSE;
	else if (flags & WSP_ABOVE)
	    before = TRUE;
	else if (flags & WSP_VERT)
	    before = !p_spr;
	else
	    before = !p_sb;
    }
    if (curfrp->fr_parent == NULL || curfrp->fr_parent->fr_layout != layout)
    {
	/* Need to create a new frame in the tree to make a branch. */
	frp = ALLOC_CLEAR_ONE(frame_T);
	*frp = *curfrp;
	curfrp->fr_layout = layout;
	frp->fr_parent = curfrp;
	frp->fr_next = NULL;
	frp->fr_prev = NULL;
	curfrp->fr_child = frp;
	curfrp->fr_win = NULL;
	curfrp = frp;
	if (frp->fr_win != NULL)
	    oldwin->w_frame = frp;
	else
	    FOR_ALL_FRAMES(frp, frp->fr_child)
		frp->fr_parent = curfrp;
    }

    if (new_wp == NULL)
	frp = wp->w_frame;
    else
	frp = new_wp->w_frame;
    frp->fr_parent = curfrp->fr_parent;

    /* Insert the new frame at the right place in the frame list. */
    if (before)
	frame_insert(curfrp, frp);
    else
	frame_append(curfrp, frp);

    /* Set w_fraction now so that the cursor keeps the same relative
     * vertical position. */
    if (!did_set_fraction)
	set_fraction(oldwin);
    wp->w_fraction = oldwin->w_fraction;

    if (flags & WSP_VERT)
    {
	wp->w_p_scr = curwin->w_p_scr;

	if (need_status)
	{
	    win_new_height(oldwin, oldwin->w_height - 1);
	    oldwin->w_status_height = need_status;
	}
	if (flags & (WSP_TOP | WSP_BOT))
	{
	    /* set height and row of new window to full height */
	    wp->w_winrow = tabline_height();
	    win_new_height(wp, curfrp->fr_height - (p_ls > 0)
							  - WINBAR_HEIGHT(wp));
	    wp->w_status_height = (p_ls > 0);
	}
	else
	{
	    /* height and row of new window is same as current window */
	    wp->w_winrow = oldwin->w_winrow;
	    win_new_height(wp, VISIBLE_HEIGHT(oldwin));
	    wp->w_status_height = oldwin->w_status_height;
	}
	frp->fr_height = curfrp->fr_height;

	/* "new_size" of the current window goes to the new window, use
	 * one column for the vertical separator */
	win_new_width(wp, new_size);
	if (before)
	    wp->w_vsep_width = 1;
	else
	{
	    wp->w_vsep_width = oldwin->w_vsep_width;
	    oldwin->w_vsep_width = 1;
	}
	if (flags & (WSP_TOP | WSP_BOT))
	{
	    if (flags & WSP_BOT)
		frame_add_vsep(curfrp);
	    /* Set width of neighbor frame */
	    frame_new_width(curfrp, curfrp->fr_width
		     - (new_size + ((flags & WSP_TOP) != 0)), flags & WSP_TOP,
								       FALSE);
	}
	else
	    win_new_width(oldwin, oldwin->w_width - (new_size + 1));
	if (before)	/* new window left of current one */
	{
	    wp->w_wincol = oldwin->w_wincol;
	    oldwin->w_wincol += new_size + 1;
	}
	else		/* new window right of current one */
	    wp->w_wincol = oldwin->w_wincol + oldwin->w_width + 1;
	frame_fix_width(oldwin);
	frame_fix_width(wp);
    }
    else
    {
	/* width and column of new window is same as current window */
	if (flags & (WSP_TOP | WSP_BOT))
	{
	    wp->w_wincol = 0;
	    win_new_width(wp, Columns);
	    wp->w_vsep_width = 0;
	}
	else
	{
	    wp->w_wincol = oldwin->w_wincol;
	    win_new_width(wp, oldwin->w_width);
	    wp->w_vsep_width = oldwin->w_vsep_width;
	}
	frp->fr_width = curfrp->fr_width;

	/* "new_size" of the current window goes to the new window, use
	 * one row for the status line */
	win_new_height(wp, new_size);
	if (flags & (WSP_TOP | WSP_BOT))
	{
	    int new_fr_height = curfrp->fr_height - new_size
							  + WINBAR_HEIGHT(wp) ;

	    if (!((flags & WSP_BOT) && p_ls == 0))
		new_fr_height -= STATUS_HEIGHT;
	    frame_new_height(curfrp, new_fr_height, flags & WSP_TOP, FALSE);
	}
	else
	    win_new_height(oldwin, oldwin_height - (new_size + STATUS_HEIGHT));
	if (before)	/* new window above current one */
	{
	    wp->w_winrow = oldwin->w_winrow;
	    wp->w_status_height = STATUS_HEIGHT;
	    oldwin->w_winrow += wp->w_height + STATUS_HEIGHT;
	}
	else		/* new window below current one */
	{
	    wp->w_winrow = oldwin->w_winrow + VISIBLE_HEIGHT(oldwin)
							       + STATUS_HEIGHT;
	    wp->w_status_height = oldwin->w_status_height;
	    if (!(flags & WSP_BOT))
		oldwin->w_status_height = STATUS_HEIGHT;
	}
	if (flags & WSP_BOT)
	    frame_add_statusline(curfrp);
	frame_fix_height(wp);
	frame_fix_height(oldwin);
    }

    if (flags & (WSP_TOP | WSP_BOT))
	(void)win_comp_pos();

    /*
     * Both windows need redrawing
     */
    redraw_win_later(wp, NOT_VALID);
    wp->w_redr_status = TRUE;
    redraw_win_later(oldwin, NOT_VALID);
    oldwin->w_redr_status = TRUE;

    if (need_status)
    {
	msg_row = Rows - 1;
	msg_col = sc_col;
	msg_clr_eos_force();	/* Old command/ruler may still be there */
	comp_col();
	msg_row = Rows - 1;
	msg_col = 0;	/* put position back at start of line */
    }

    /*
     * equalize the window sizes.
     */
    if (do_equal || dir != 0)
	win_equal(wp, TRUE,
		(flags & WSP_VERT) ? (dir == 'v' ? 'b' : 'h')
		: dir == 'h' ? 'b' : 'v');

    /* Don't change the window height/width to 'winheight' / 'winwidth' if a
     * size was given. */
    if (flags & WSP_VERT)
    {
	i = p_wiw;
	if (size != 0)
	    p_wiw = size;

# ifdef FEAT_GUI
	/* When 'guioptions' includes 'L' or 'R' may have to add scrollbars. */
	if (gui.in_use)
	    gui_init_which_components(NULL);
# endif
    }
    else
    {
	i = p_wh;
	if (size != 0)
	    p_wh = size;
    }

#ifdef FEAT_JUMPLIST
    /* Keep same changelist position in new window. */
    wp->w_changelistidx = oldwin->w_changelistidx;
#endif

    /*
     * make the new window the current window
     */
    win_enter_ext(wp, FALSE, FALSE, TRUE, TRUE, TRUE);
    if (flags & WSP_VERT)
	p_wiw = i;
    else
	p_wh = i;

    return OK;
}