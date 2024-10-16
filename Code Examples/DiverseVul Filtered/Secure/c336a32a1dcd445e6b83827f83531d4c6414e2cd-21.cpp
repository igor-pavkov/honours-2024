static void MapCharsetR(int n)
{
	curr->w_ss = 0;
	if (curr->w_CharsetR != n) {
		curr->w_CharsetR = n;
		curr->w_FontR = curr->w_charsets[n];
	}
	curr->w_gr = 1;
}