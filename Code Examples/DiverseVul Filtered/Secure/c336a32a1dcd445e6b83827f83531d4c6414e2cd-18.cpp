static void CursorUp(int n)
{
	if (curr->w_y < curr->w_top) {	/* if above scrolling rgn, */
		if ((curr->w_y -= n) < 0)	/* ignore its limits      */
			curr->w_y = 0;
	} else if ((curr->w_y -= n) < curr->w_top)
		curr->w_y = curr->w_top;
	LGotoPos(&curr->w_layer, curr->w_x, curr->w_y);
}