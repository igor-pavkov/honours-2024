static void ClearScreen()
{
	LClearArea(&curr->w_layer, 0, 0, curr->w_width - 1, curr->w_height - 1, curr->w_rend.colorbg, 1);
	MScrollV(curr, curr->w_height, 0, curr->w_height - 1, curr->w_rend.colorbg);
}