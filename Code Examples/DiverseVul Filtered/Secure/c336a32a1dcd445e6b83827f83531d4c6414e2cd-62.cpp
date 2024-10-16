static void DesignateCharset(int c, int n)
{
	curr->w_ss = 0;
	if (c == ('@' & 037))	/* map JIS 6226 to 0208 */
		c = KANJI;
	if (c == 'B')
		c = ASCII;
	if (curr->w_charsets[n] != c) {
		curr->w_charsets[n] = c;
		if (curr->w_Charset == n) {
			curr->w_FontL = c;
			curr->w_rend.font = curr->w_FontL;
			LSetRendition(&curr->w_layer, &curr->w_rend);
		}
		if (curr->w_CharsetR == n)
			curr->w_FontR = c;
	}
}