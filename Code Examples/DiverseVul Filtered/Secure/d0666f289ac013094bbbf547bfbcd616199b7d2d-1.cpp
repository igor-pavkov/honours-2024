int EVP_EncodeBlock(unsigned char *t, const unsigned char *f, int dlen)
	{
	int i,ret=0;
	unsigned long l;

	for (i=dlen; i > 0; i-=3)
		{
		if (i >= 3)
			{
			l=	(((unsigned long)f[0])<<16L)|
				(((unsigned long)f[1])<< 8L)|f[2];
			*(t++)=conv_bin2ascii(l>>18L);
			*(t++)=conv_bin2ascii(l>>12L);
			*(t++)=conv_bin2ascii(l>> 6L);
			*(t++)=conv_bin2ascii(l     );
			}
		else
			{
			l=((unsigned long)f[0])<<16L;
			if (i == 2) l|=((unsigned long)f[1]<<8L);

			*(t++)=conv_bin2ascii(l>>18L);
			*(t++)=conv_bin2ascii(l>>12L);
			*(t++)=(i == 1)?'=':conv_bin2ascii(l>> 6L);
			*(t++)='=';
			}
		ret+=4;
		f+=3;
		}

	*t='\0';
	return(ret);
	}