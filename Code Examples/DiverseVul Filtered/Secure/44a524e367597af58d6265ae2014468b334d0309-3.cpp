static int mem_resize(jas_stream_memobj_t *m, int bufsize)
{
	unsigned char *buf;

	//assert(m->buf_);
	assert(bufsize >= 0);
	if (!(buf = jas_realloc2(m->buf_, bufsize, sizeof(unsigned char))) &&
	  bufsize) {
		return -1;
	}
	m->buf_ = buf;
	m->bufsize_ = bufsize;
	return 0;
}