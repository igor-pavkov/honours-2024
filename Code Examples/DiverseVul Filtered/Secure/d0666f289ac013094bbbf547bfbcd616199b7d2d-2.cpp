int EVP_DecodeBlock(unsigned char *t, const unsigned char *f, int n)
	{
	int i,ret=0,a,b,c,d;
	unsigned long l;

	/* trim white space from the start of the line. */
	while ((conv_ascii2bin(*f) == B64_WS) && (n > 0))
		{
		f++;
		n--;
		}

	/* strip off stuff at the end of the line
	 * ascii2bin values B64_WS, B64_EOLN, B64_EOLN and B64_EOF */
	while ((n > 3) && (B64_NOT_BASE64(conv_ascii2bin(f[n-1]))))
		n--;

	if (n%4 != 0) return(-1);

	for (i=0; i<n; i+=4)
		{
		a=conv_ascii2bin(*(f++));
		b=conv_ascii2bin(*(f++));
		c=conv_ascii2bin(*(f++));
		d=conv_ascii2bin(*(f++));
		if (	(a & 0x80) || (b & 0x80) ||
			(c & 0x80) || (d & 0x80))
			return(-1);
		l=(	(((unsigned long)a)<<18L)|
			(((unsigned long)b)<<12L)|
			(((unsigned long)c)<< 6L)|
			(((unsigned long)d)     ));
		*(t++)=(unsigned char)(l>>16L)&0xff;
		*(t++)=(unsigned char)(l>> 8L)&0xff;
		*(t++)=(unsigned char)(l     )&0xff;
		ret+=3;
		}
	return(ret);
	}