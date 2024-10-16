static void MClearArea(Window *p, int xs, int ys, int xe, int ye, int bce)
{
	int n, y;
	int xxe;
	struct mline *ml;

	/* Check for zero-height window */
	if (ys < 0 || ye < ys)
		return;

	/* check for magic margin condition */
	if (xs >= p->w_width)
		xs = p->w_width - 1;
	if (xe >= p->w_width)
		xe = p->w_width - 1;

	MKillDwRight(p, p->w_mlines + ys, xs);
	MKillDwLeft(p, p->w_mlines + ye, xe);

	ml = p->w_mlines + ys;
	for (y = ys; y <= ye; y++, ml++) {
		xxe = (y == ye) ? xe : p->w_width - 1;
		n = xxe - xs + 1;
		if (n > 0)
			clear_mline(ml, xs, n);
		if (n > 0 && bce)
			MBceLine(p, y, xs, xs + n - 1, bce);
		xs = 0;
	}
}