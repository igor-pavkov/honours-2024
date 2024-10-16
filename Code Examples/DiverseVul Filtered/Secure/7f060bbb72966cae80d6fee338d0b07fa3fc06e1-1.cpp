static u32 avi_write (FILE *fd, char *buf, u32 len)
{
	u32 r = 0;

	while (r < len) {
		s32 n = (u32) gf_fwrite (buf + r, len - r, fd);
		if (n < 0)
			return n;

		r += n;
	}
	return r;
}