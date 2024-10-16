static void ClearToEOS()
{
	int y = curr->w_y, x = curr->w_x;

	if (x == 0 && y == 0) {
		ClearScreen();
		RestorePosRendition();
		return;
	}
	LClearArea(&curr->w_layer, x, y, cols - 1, rows - 1, curr->w_rend.colorbg, 1);
	MClearArea(curr, x, y, cols - 1, rows - 1, curr->w_rend.colorbg);
	RestorePosRendition();
}