void ResetWindow(Window *p)
{
	int i;

	p->w_wrap = nwin_default.wrap;
	p->w_origin = 0;
	p->w_insert = 0;
	p->w_revvid = 0;
	p->w_mouse = 0;
	p->w_bracketed = 0;
	p->w_cursorstyle = 0;
	p->w_curinv = 0;
	p->w_curvvis = 0;
	p->w_autolf = 0;
	p->w_keypad = 0;
	p->w_cursorkeys = 0;
	p->w_top = 0;
	p->w_bot = p->w_height - 1;
	p->w_saved.on = 0;
	p->w_x = p->w_y = 0;
	p->w_state = LIT;
	p->w_StringType = NONE;
	memset(p->w_tabs, 0, p->w_width);
	for (i = 8; i < p->w_width; i += 8)
		p->w_tabs[i] = 1;
	p->w_rend = mchar_null;
	ResetCharsets(p);
}