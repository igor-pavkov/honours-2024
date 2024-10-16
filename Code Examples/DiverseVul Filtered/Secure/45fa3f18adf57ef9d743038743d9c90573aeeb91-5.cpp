static char *base64enc(const char *str)
{
	/* paranoia */
	unsigned len = strnlen(str, sizeof(G.wget_buf)/4*3 - 10);
	bb_uuencode(G.wget_buf, str, len, bb_uuenc_tbl_base64);
	return G.wget_buf;
}