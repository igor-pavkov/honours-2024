int uname(struct utsname *buf)
{
	unsigned v = (unsigned)GetVersion();
	memset(buf, 0, sizeof(*buf));
	xsnprintf(buf->sysname, sizeof(buf->sysname), "Windows");
	xsnprintf(buf->release, sizeof(buf->release),
		 "%u.%u", v & 0xff, (v >> 8) & 0xff);
	/* assuming NT variants only.. */
	xsnprintf(buf->version, sizeof(buf->version),
		  "%u", (v >> 16) & 0x7fff);
	return 0;
}