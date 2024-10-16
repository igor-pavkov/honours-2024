static void MWrapChar(Window *p, struct mchar *c, int y, int top, int bot, int ins)
{
	struct mline *ml;
	int bce;

	bce = c->colorbg;
	MFixLine(p, y, c);
	ml = &p->w_mlines[y];
	copy_mchar2mline(&mchar_null, ml, p->w_width);
	if (y == bot)
		MScrollV(p, 1, top, bot, bce);
	else if (y < p->w_height - 1)
		y++;
	if (ins)
		MInsChar(p, c, 0, y);
	else
		MPutChar(p, c, 0, y);
}