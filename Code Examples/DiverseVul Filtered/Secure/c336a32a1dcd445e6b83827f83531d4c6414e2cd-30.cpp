void WindowChanged(Window *p, int what)
{
	int inwstr, inhstr, inlstr;
	int inwstrh = 0, inhstrh = 0, inlstrh = 0;
	int got, ox, oy;
	Display *olddisplay = display;
	Canvas *cv;

	if (what == 'f') {
		WindowChanged((Window *)0, 'w' | 0x100);
		WindowChanged((Window *)0, 'W' | 0x100);
	}

	if (what) {
		inwstr = WindowChangedCheck(captionstring, what, &inwstrh);
		inhstr = WindowChangedCheck(hstatusstring, what, &inhstrh);
		inlstr = WindowChangedCheck(wliststr, what, &inlstrh);
	} else {
		inwstr = inhstr = 0;
		inlstr = 1;
	}

	if (p == 0) {
		for (display = displays; display; display = display->d_next) {
			ox = D_x;
			oy = D_y;
			for (cv = D_cvlist; cv; cv = cv->c_next) {
				if (inlstr
				    || (inlstrh && p && p->w_hstatus && *p->w_hstatus
					&& WindowChangedCheck(p->w_hstatus, what, (int *)0)))
					WListUpdatecv(cv, (Window *)0);
				p = Layer2Window(cv->c_layer);
				if (inwstr
				    || (inwstrh && p && p->w_hstatus && *p->w_hstatus
					&& WindowChangedCheck(p->w_hstatus, what, (int *)0))) {
					if (captiontop) {
						if (cv->c_ys - 1 >= 0)
							RefreshLine(cv->c_ys - 1, 0, D_width -1 , 0);
					} else {
						if (cv->c_ye + 1 < D_height)
							RefreshLine(cv->c_ye + 1, 0, D_width - 1, 0);
					}
				}
			}
			p = D_fore;
			if (inhstr
			    || (inhstrh && p && p->w_hstatus && *p->w_hstatus
				&& WindowChangedCheck(p->w_hstatus, what, (int *)0)))
				RefreshHStatus();
			if (ox != -1 && oy != -1)
				GotoPos(ox, oy);
		}
		display = olddisplay;
		return;
	}

	if (p->w_hstatus && *p->w_hstatus && (inwstrh || inhstrh || inlstrh)
	    && WindowChangedCheck(p->w_hstatus, what, (int *)0)) {
		inwstr |= inwstrh;
		inhstr |= inhstrh;
		inlstr |= inlstrh;
	}
	if (!inwstr && !inhstr && !inlstr)
		return;
	for (display = displays; display; display = display->d_next) {
		got = 0;
		ox = D_x;
		oy = D_y;
		for (cv = D_cvlist; cv; cv = cv->c_next) {
			if (inlstr)
				WListUpdatecv(cv, p);
			if (Layer2Window(cv->c_layer) != p)
				continue;
			got = 1;
			if (inwstr) {
				if (captiontop) {
					if (cv->c_ys -1 >= 0)
						RefreshLine(cv->c_ys - 1, 0, D_width - 1, 0);
				} else {
					if (cv->c_ye + 1 < D_height)
						RefreshLine(cv->c_ye + 1, 0, D_width - 1, 0);
				}
			}
		}
		if (got && inhstr && p == D_fore)
			RefreshHStatus();
		if (ox != -1 && oy != -1)
			GotoPos(ox, oy);
	}
	display = olddisplay;
}