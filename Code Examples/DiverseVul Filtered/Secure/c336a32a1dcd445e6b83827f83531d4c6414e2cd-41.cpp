static void MFixLine(Window *p, int y, struct mchar *mc)
{
	struct mline *ml = &p->w_mlines[y];
	if (mc->attr && ml->attr == null) {
		if ((ml->attr = calloc(p->w_width + 1, 4)) == 0) {
			ml->attr = null;
			mc->attr = p->w_rend.attr = 0;
			WMsg(p, 0, "Warning: no space for attr - turned off");
		}
	}
	if (mc->font && ml->font == null) {
		if ((ml->font = calloc(p->w_width + 1, 4)) == 0) {
			ml->font = null;
			p->w_FontL = p->w_charsets[p->w_ss ? p->w_ss : p->w_Charset] = 0;
			p->w_FontR = p->w_charsets[p->w_ss ? p->w_ss : p->w_CharsetR] = 0;
			mc->font = mc->fontx = p->w_rend.font = 0;
			WMsg(p, 0, "Warning: no space for font - turned off");
		}
	}
	if (mc->fontx && ml->fontx == null) {
		if ((ml->fontx = calloc(p->w_width + 1, 4)) == 0) {
			ml->fontx = null;
			mc->fontx = 0;
		}
	}
	if (mc->colorbg && ml->colorbg == null) {
		if ((ml->colorbg = calloc(p->w_width + 1, 4)) == 0) {
			ml->colorbg = null;
			mc->colorbg = p->w_rend.colorbg = 0;
			WMsg(p, 0, "Warning: no space for color background - turned off");
		}
	}
	if (mc->colorfg && ml->colorfg == null) {
		if ((ml->colorfg = calloc(p->w_width + 1, 4)) == 0) {
			ml->colorfg = null;
			mc->colorfg = p->w_rend.colorfg = 0;
			WMsg(p, 0, "Warning: no space for color foreground - turned off");
		}
	}
}