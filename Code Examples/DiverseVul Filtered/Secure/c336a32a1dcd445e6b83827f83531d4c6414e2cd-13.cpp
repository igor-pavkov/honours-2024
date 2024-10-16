static void MScrollH(Window *p, int n, int y, int xs, int xe, int bce)
{
	struct mline *ml;

	if (n == 0)
		return;
	ml = &p->w_mlines[y];
	MKillDwRight(p, ml, xs);
	MKillDwLeft(p, ml, xe);
	if (n > 0) {
		if (xe - xs + 1 > n) {
			MKillDwRight(p, ml, xs + n);
			copy_mline(ml, xs + n, xs, xe + 1 - xs - n);
		} else
			n = xe - xs + 1;
		clear_mline(ml, xe + 1 - n, n);
		if (bce)
			MBceLine(p, y, xe + 1 - n, n, bce);
	} else {
		n = -n;
		if (xe - xs + 1 > n) {
			MKillDwLeft(p, ml, xe - n);
			copy_mline(ml, xs, xs + n, xe + 1 - xs - n);
		} else
			n = xe - xs + 1;
		clear_mline(ml, xs, n);
		if (bce)
			MBceLine(p, y, xs, n, bce);
	}
}