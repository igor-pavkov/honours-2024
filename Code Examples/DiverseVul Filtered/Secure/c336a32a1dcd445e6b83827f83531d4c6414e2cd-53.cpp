static void RestorePosRendition()
{
	LGotoPos(&curr->w_layer, curr->w_x, curr->w_y);
	LSetRendition(&curr->w_layer, &curr->w_rend);
}