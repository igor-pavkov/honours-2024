static void Report(char *fmt, int n1, int n2)
{
	int len;
	char rbuf[40];		/* enough room for all replys */

	sprintf(rbuf, fmt, n1, n2);
	len = strlen(rbuf);

	if (W_UWP(curr)) {
		if ((unsigned)(curr->w_pwin->p_inlen + len) <= sizeof(curr->w_pwin->p_inbuf)) {
			memmove(curr->w_pwin->p_inbuf + curr->w_pwin->p_inlen, rbuf, len);
			curr->w_pwin->p_inlen += len;
		}
	} else {
		if ((unsigned)(curr->w_inlen + len) <= sizeof(curr->w_inbuf)) {
			memmove(curr->w_inbuf + curr->w_inlen, rbuf, len);
			curr->w_inlen += len;
		}
	}
}