static void ReverseLineFeed()
{
	if (curr->w_y == curr->w_top) {
		MScrollV(curr, -1, curr->w_top, curr->w_bot, curr->w_rend.colorbg);
		LScrollV(&curr->w_layer, -1, curr->w_top, curr->w_bot, curr->w_rend.colorbg);
		LGotoPos(&curr->w_layer, curr->w_x, curr->w_y);
	} else if (curr->w_y > 0)
		CursorUp(1);
}