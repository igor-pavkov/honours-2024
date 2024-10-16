int EVP_DecodeValid(unsigned char *buf, int len)
	{
	int i,num=0,bad=0;

	if (len == 0) return(-1);
	while (conv_ascii2bin(*buf) == B64_WS)
		{
		buf++;
		len--;
		if (len == 0) return(-1);
		}

	for (i=len; i >= 4; i-=4)
		{
		if (	(conv_ascii2bin(buf[0]) >= 0x40) ||
			(conv_ascii2bin(buf[1]) >= 0x40) ||
			(conv_ascii2bin(buf[2]) >= 0x40) ||
			(conv_ascii2bin(buf[3]) >= 0x40))
			return(-1);
		buf+=4;
		num+=1+(buf[2] != '=')+(buf[3] != '=');
		}
	if ((i == 1) && (conv_ascii2bin(buf[0]) == B64_EOLN))
		return(num);
	if ((i == 2) && (conv_ascii2bin(buf[0]) == B64_EOLN) &&
		(conv_ascii2bin(buf[0]) == B64_EOLN))
		return(num);
	return(1);
	}