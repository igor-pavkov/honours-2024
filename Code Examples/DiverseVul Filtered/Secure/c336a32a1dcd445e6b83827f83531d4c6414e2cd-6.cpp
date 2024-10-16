static void ClearFromBOS()
{
	int y = curr->w_y, x = curr->w_x;

	LClearArea(&curr->w_layer, 0, 0, x, y, curr->w_rend.colorbg, 1);
	MClearArea(curr, 0, 0, x, y, curr->w_rend.colorbg);
	RestorePosRendition();
}