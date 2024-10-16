static int dynamicGetchar (gdIOCtxPtr ctx)
{
	unsigned char b;
	int rv;

	rv = dynamicGetbuf (ctx, &b, 1);
	if (rv != 1) {
		return EOF;
	} else {
		return b; 		/* (b & 0xff); */
	}
}