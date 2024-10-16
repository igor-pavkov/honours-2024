static void FillWithEs()
{
	int i;
	uint32_t *p, *ep;

	LClearAll(&curr->w_layer, 1);
	curr->w_y = curr->w_x = 0;
	for (i = 0; i < rows; ++i) {
		clear_mline(&curr->w_mlines[i], 0, cols + 1);
		p = curr->w_mlines[i].image;
		ep = p + cols;
		while (p < ep)
			*p++ = 'E';
	}
	LRefreshAll(&curr->w_layer, 1);
}