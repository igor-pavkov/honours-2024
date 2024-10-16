void WBell(Window *p, int visual)
{
	Canvas *cv;
	if (displays == NULL)
		p->w_bell = BELL_DONE;
	for (display = displays; display; display = display->d_next) {
		for (cv = D_cvlist; cv; cv = cv->c_next)
			if (cv->c_layer->l_bottom == &p->w_layer)
				break;
		if (cv && !visual)
			AddCStr(D_BL);
		else if (cv && D_VB)
			AddCStr(D_VB);
		else
			p->w_bell = visual ? BELL_VISUAL : BELL_FOUND;
	}
}