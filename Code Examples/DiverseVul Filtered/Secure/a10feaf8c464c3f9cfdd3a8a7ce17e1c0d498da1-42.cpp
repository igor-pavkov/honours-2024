static int dvb_filter_pes2ts(struct dvb_filter_pes2ts *p2ts,
			     unsigned char *pes, int len, int payload_start)
{
	unsigned char *buf=p2ts->buf;
	int ret=0, rest;

	//len=6+((pes[4]<<8)|pes[5]);

	if (payload_start)
		buf[1]|=0x40;
	else
		buf[1]&=~0x40;
	while (len>=184) {
		buf[3]=0x10|((p2ts->cc++)&0x0f);
		memcpy(buf+4, pes, 184);
		if ((ret=p2ts->cb(p2ts->priv, buf)))
			return ret;
		len-=184; pes+=184;
		buf[1]&=~0x40;
	}
	if (!len)
		return 0;
	buf[3]=0x30|((p2ts->cc++)&0x0f);
	rest=183-len;
	if (rest) {
		buf[5]=0x00;
		if (rest-1)
			memset(buf+6, 0xff, rest-1);
	}
	buf[4]=rest;
	memcpy(buf+5+rest, pes, len);
	return p2ts->cb(p2ts->priv, buf);
}