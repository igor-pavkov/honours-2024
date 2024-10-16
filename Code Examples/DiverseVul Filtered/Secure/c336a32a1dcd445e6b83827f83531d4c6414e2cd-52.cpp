static void BackwardTab()
{
	int x = curr->w_x;

	if (curr->w_tabs[x] && x > 0)
		x--;
	while (x > 0 && !curr->w_tabs[x])
		x--;
	curr->w_x = x;
	LGotoPos(&curr->w_layer, curr->w_x, curr->w_y);
}