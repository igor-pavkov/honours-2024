void WChangeSize(Window *p, int w, int h)
{
	int wok = 0;
	Canvas *cv;

	if (p->w_layer.l_cvlist == 0) {
		/* window not displayed -> works always */
		ChangeWindowSize(p, w, h, p->w_histheight);
		return;
	}
	for (cv = p->w_layer.l_cvlist; cv; cv = cv->c_lnext) {
		display = cv->c_display;
		if (p != D_fore)
			continue;	/* change only fore */
		if (D_CWS)
			break;
		if (D_CZ0 && (w == Z0width || w == Z1width))
			wok = 1;
	}
	if (cv == 0 && wok == 0)	/* can't change any display */
		return;
	if (!D_CWS)
		h = p->w_height;
	ChangeWindowSize(p, w, h, p->w_histheight);
	for (display = displays; display; display = display->d_next) {
		if (p == D_fore) {
			if (D_cvlist && D_cvlist->c_next == 0)
				ResizeDisplay(w, h);
			else
				ResizeDisplay(w, D_height);
			ResizeLayersToCanvases();	/* XXX Hmm ? */
			continue;
		}
		for (cv = D_cvlist; cv; cv = cv->c_next)
			if (cv->c_layer->l_bottom == &p->w_layer)
				break;
		if (cv)
			Redisplay(0);
	}
}