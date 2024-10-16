static void CursorRight(int n)
{
	int x = curr->w_x;

	if (x == cols)
		LineFeed(1);
	if ((curr->w_x += n) >= cols)
		curr->w_x = cols - 1;
	LGotoPos(&curr->w_layer, curr->w_x, curr->w_y);
}