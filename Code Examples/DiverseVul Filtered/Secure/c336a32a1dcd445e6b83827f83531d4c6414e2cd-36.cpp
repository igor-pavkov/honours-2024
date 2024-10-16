static void WReverseVideo(Window *p, int on)
{
	Canvas *cv;
	for (cv = p->w_layer.l_cvlist; cv; cv = cv->c_lnext) {
		display = cv->c_display;
		if (cv != D_forecv)
			continue;
		ReverseVideo(on);
		if (!on && p->w_revvid && !D_CVR) {
			if (D_VB)
				AddCStr(D_VB);
			else
				p->w_bell = BELL_VISUAL;
		}
	}
}