static void MPutChar(Window *p, struct mchar *c, int x, int y)
{
	struct mline *ml;

	MFixLine(p, y, c);
	ml = &p->w_mlines[y];
	MKillDwRight(p, ml, x);
	MKillDwLeft(p, ml, x);
	copy_mchar2mline(c, ml, x);
	if (c->mbcs) {
		MKillDwLeft(p, ml, x + 1);
		copy_mchar2mline(c, ml, x + 1);
		ml->image[x + 1] = c->mbcs;
		if (p->w_encoding != UTF8)
			ml->font[x + 1] |= 0x80;
		else if (p->w_encoding == UTF8 && c->mbcs) {
			ml->font[x + 1] = c->mbcs;
			ml->fontx[x + 1] = 0;
		}
	}
}