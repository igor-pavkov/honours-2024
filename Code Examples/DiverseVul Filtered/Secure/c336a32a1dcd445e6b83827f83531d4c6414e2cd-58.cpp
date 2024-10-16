static void MapCharset(int n)
{
	curr->w_ss = 0;
	if (curr->w_Charset != n) {
		curr->w_Charset = n;
		curr->w_FontL = curr->w_charsets[n];
		curr->w_rend.font = curr->w_FontL;
		LSetRendition(&curr->w_layer, &curr->w_rend);
	}
}