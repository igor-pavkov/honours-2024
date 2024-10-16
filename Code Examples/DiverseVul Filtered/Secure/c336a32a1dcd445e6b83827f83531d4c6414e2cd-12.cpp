int MFindUsedLine(Window *p, int ye, int ys)
{
	int y;
	struct mline *ml = p->w_mlines + ye;

	for (y = ye; y >= ys; y--, ml--) {
		if (memcmp(ml->image, blank, p->w_width * 4))
			break;
		if (ml->attr != null && memcmp(ml->attr, null, p->w_width * 4))
			break;
		if (ml->colorbg != null && memcmp(ml->colorbg, null, p->w_width * 4))
			break;
		if (ml->colorfg != null && memcmp(ml->colorfg, null, p->w_width * 4))
			break;
		if (p->w_encoding == UTF8) {
			if (ml->font != null && bcmp((char *)ml->font, null, p->w_width))
				break;
			if (ml->fontx != null && bcmp((char *)ml->fontx, null, p->w_width))
				break;
		}
	}
	return y;
}