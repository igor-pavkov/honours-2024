static u32 avi_read(FILE *fd, char *buf, u32 len)
{
	u32 r = 0;

	while (r < len) {
		s32 n = (s32) gf_fread(buf + r, len - r, fd);
		if (n == 0) break;
		if (n < 0) return r;
		r += n;
	}

	return r;
}