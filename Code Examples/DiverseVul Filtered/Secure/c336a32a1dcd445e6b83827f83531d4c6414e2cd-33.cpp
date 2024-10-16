static void PrintStart()
{
	curr->w_pdisplay = 0;

	/* find us a nice display to print on, fore prefered */
	display = curr->w_lastdisp;
	if (!(display && curr == D_fore && (printcmd || D_PO)))
		for (display = displays; display; display = display->d_next)
			if (curr == D_fore && (printcmd || D_PO))
				break;
	if (!display) {
		Canvas *cv;
		for (cv = curr->w_layer.l_cvlist; cv; cv = cv->c_lnext) {
			display = cv->c_display;
			if (printcmd || D_PO)
				break;
		}
		if (!cv) {
			display = displays;
			if (!display || display->d_next || !(printcmd || D_PO))
				return;
		}
	}
	curr->w_pdisplay = display;
	curr->w_stringp = curr->w_string;
	curr->w_state = PRIN;
	if (printcmd && curr->w_pdisplay->d_printfd < 0)
		curr->w_pdisplay->d_printfd = printpipe(curr, printcmd);
}