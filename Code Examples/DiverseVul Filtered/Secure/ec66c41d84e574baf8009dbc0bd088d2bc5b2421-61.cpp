frame_add_vsep(frame_T *frp)
{
    win_T	*wp;

    if (frp->fr_layout == FR_LEAF)
    {
	wp = frp->fr_win;
	if (wp->w_vsep_width == 0)
	{
	    if (wp->w_width > 0)	/* don't make it negative */
		--wp->w_width;
	    wp->w_vsep_width = 1;
	}
    }
    else if (frp->fr_layout == FR_COL)
    {
	/* Handle all the frames in the column. */
	FOR_ALL_FRAMES(frp, frp->fr_child)
	    frame_add_vsep(frp);
    }
    else /* frp->fr_layout == FR_ROW */
    {
	/* Only need to handle the last frame in the row. */
	frp = frp->fr_child;
	while (frp->fr_next != NULL)
	    frp = frp->fr_next;
	frame_add_vsep(frp);
    }
}