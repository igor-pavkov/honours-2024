static void MInsChar(Window *p, struct mchar *c, int x, int y)
{
	int n;
	struct mline *ml;

	MFixLine(p, y, c);
	ml = p->w_mlines + y;
	n = p->w_width - x - 1;
	MKillDwRight(p, ml, x);
	if (n > 0) {
		MKillDwRight(p, ml, p->w_width - 1);
		copy_mline(ml, x, x + 1, n);
	}
	copy_mchar2mline(c, ml, x);
	if (c->mbcs) {
		if (--n > 0) {
			MKillDwRight(p, ml, p->w_width - 1);
			copy_mline(ml, x + 1, x + 2, n);
		}
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