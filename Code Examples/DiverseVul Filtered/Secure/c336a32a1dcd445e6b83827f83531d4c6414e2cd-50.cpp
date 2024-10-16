static void CursorLeft(int n)
{
	if ((curr->w_x -= n) < 0)
		curr->w_x = 0;
	LGotoPos(&curr->w_layer, curr->w_x, curr->w_y);
}