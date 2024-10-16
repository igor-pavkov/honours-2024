static void PrintFlush()
{
	display = curr->w_pdisplay;
	if (display && printcmd) {
		char *bp = curr->w_string;
		int len = curr->w_stringp - curr->w_string;
		int r;
		while (len && display->d_printfd >= 0) {
			r = write(display->d_printfd, bp, len);
			if (r <= 0) {
				WMsg(curr, errno, "printing aborted");
				close(display->d_printfd);
				display->d_printfd = -1;
				break;
			}
			bp += r;
			len -= r;
		}
	} else if (display && curr->w_stringp > curr->w_string) {
		AddCStr(D_PO);
		AddStrn(curr->w_string, curr->w_stringp - curr->w_string);
		AddCStr(D_PF);
		Flush(3);
	}
	curr->w_stringp = curr->w_string;
}