static void LineFeed(int out_mode)
{
	/* out_mode: 0=lf, 1=cr+lf */
	if (out_mode)
		curr->w_x = 0;
	if (curr->w_y != curr->w_bot) {	/* Don't scroll */
		if (curr->w_y < rows - 1)
			curr->w_y++;
		LGotoPos(&curr->w_layer, curr->w_x, curr->w_y);
		return;
	}
	if (curr->w_autoaka > 1)
		curr->w_autoaka--;
	MScrollV(curr, 1, curr->w_top, curr->w_bot, curr->w_rend.colorbg);
	LScrollV(&curr->w_layer, 1, curr->w_top, curr->w_bot, curr->w_rend.colorbg);
	LGotoPos(&curr->w_layer, curr->w_x, curr->w_y);
}