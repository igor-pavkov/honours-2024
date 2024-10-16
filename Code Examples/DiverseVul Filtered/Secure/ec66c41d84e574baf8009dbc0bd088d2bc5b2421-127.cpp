frame_add_statusline(frame_T *frp)
{
    win_T	*wp;

    if (frp->fr_layout == FR_LEAF)
    {
	wp = frp->fr_win;
	if (wp->w_status_height == 0)
	{
	    if (wp->w_height > 0)	/* don't make it negative */
		--wp->w_height;
	    wp->w_status_height = STATUS_HEIGHT;
	}
    }
    else if (frp->fr_layout == FR_ROW)
    {
	/* Handle all the frames in the row. */
	FOR_ALL_FRAMES(frp, frp->fr_child)
	    frame_add_statusline(frp);
    }
    else /* frp->fr_layout == FR_COL */
    {
	/* Only need to handle the last frame in the column. */
	for (frp = frp->fr_child; frp->fr_next != NULL; frp = frp->fr_next)
	    ;
	frame_add_statusline(frp);
    }
}