static void Return()
{
	if (curr->w_x == 0)
		return;
	curr->w_x = 0;
	LGotoPos(&curr->w_layer, curr->w_x, curr->w_y);
}