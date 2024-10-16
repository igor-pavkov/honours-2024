static void ForwardTab()
{
	int x = curr->w_x;

	if (x == cols) {
		LineFeed(1);
		x = 0;
	}
	if (curr->w_tabs[x] && x < cols - 1)
		x++;
	while (x < cols - 1 && !curr->w_tabs[x])
		x++;
	curr->w_x = x;
	LGotoPos(&curr->w_layer, curr->w_x, curr->w_y);
}