static void ScrollRegion(int n)
{
	MScrollV(curr, n, curr->w_top, curr->w_bot, curr->w_rend.colorbg);
	LScrollV(&curr->w_layer, n, curr->w_top, curr->w_bot, curr->w_rend.colorbg);
	LGotoPos(&curr->w_layer, curr->w_x, curr->w_y);
}