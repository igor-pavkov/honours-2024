static void CursorDown(int n)
{
	if (curr->w_y > curr->w_bot) {	/* if below scrolling rgn, */
		if ((curr->w_y += n) > rows - 1)	/* ignore its limits      */
			curr->w_y = rows - 1;
	} else if ((curr->w_y += n) > curr->w_bot)
		curr->w_y = curr->w_bot;
	LGotoPos(&curr->w_layer, curr->w_x, curr->w_y);
}