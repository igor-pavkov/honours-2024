size_t _cdk_pkt_read_len(FILE * inp, size_t * ret_partial)
{
	int c1, c2;
	size_t pktlen;

	c1 = fgetc(inp);
	if (c1 == EOF)
		return (size_t) EOF;
	if (c1 < 224 || c1 == 255)
		*ret_partial = 0;	/* End of partial data */
	if (c1 < 192)
		pktlen = c1;
	else if (c1 >= 192 && c1 <= 223) {
		c2 = fgetc(inp);
		if (c2 == EOF)
			return (size_t) EOF;
		pktlen = ((c1 - 192) << 8) + c2 + 192;
	} else if (c1 == 255) {
		pktlen = fgetc(inp) << 24;
		pktlen |= fgetc(inp) << 16;
		pktlen |= fgetc(inp) << 8;
		pktlen |= fgetc(inp) << 0;
	} else
		pktlen = 1 << (c1 & 0x1f);
	return pktlen;
}