static void MBceLine(Window *p, int y, int xs, int xe, int bce)
{
	struct mchar mc;
	struct mline *ml;
	int x;

	mc = mchar_null;
	mc.colorbg = bce;
	MFixLine(p, y, &mc);
	ml = p->w_mlines + y;
	if (mc.attr)
		for (x = xs; x <= xe; x++)
			ml->attr[x] = mc.attr;
	if (mc.colorbg)
		for (x = xs; x <= xe; x++)
			ml->colorbg[x] = mc.colorbg;
	if (mc.colorfg)
		for (x = xs; x <= xe; x++)
			ml->colorfg[x] = mc.colorfg;
}