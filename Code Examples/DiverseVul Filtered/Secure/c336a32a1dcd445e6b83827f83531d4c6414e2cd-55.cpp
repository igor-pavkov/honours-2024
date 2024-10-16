static void ClearLineRegion(int from, int to)
{
	int y = curr->w_y;
	LClearArea(&curr->w_layer, from, y, to, y, curr->w_rend.colorbg, 1);
	MClearArea(curr, from, y, to, y, curr->w_rend.colorbg);
	RestorePosRendition();
}