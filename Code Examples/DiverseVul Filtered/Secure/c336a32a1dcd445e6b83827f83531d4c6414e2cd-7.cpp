static void InsertLine(int n)
{
	if (curr->w_y < curr->w_top || curr->w_y > curr->w_bot)
		return;
	if (n > curr->w_bot - curr->w_y + 1)
		n = curr->w_bot - curr->w_y + 1;
	MScrollV(curr, -n, curr->w_y, curr->w_bot, curr->w_rend.colorbg);
	LScrollV(&curr->w_layer, -n, curr->w_y, curr->w_bot, curr->w_rend.colorbg);
	LGotoPos(&curr->w_layer, curr->w_x, curr->w_y);
}