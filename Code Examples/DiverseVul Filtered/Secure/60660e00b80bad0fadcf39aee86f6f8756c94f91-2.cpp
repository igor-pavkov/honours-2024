static int ofv_append_varr(struct ofv_varr *p, const char *x)
{
	if (p->off + 1 >= p->cap) {
		const char **ndata;
		unsigned int ncap = (p->off + 1) * 2;
		if (p->off + 1 >= ncap) {
			log_error("%s: ncap exceeded\n", __func__);
			return 1;
		};
		ndata = realloc(p->data, ncap * sizeof(const char *));
		if (ndata) {
			p->data = ndata;
			p->cap = ncap;
		} else {
			log_error("realloc: %s\n", strerror(errno));
			return 1;
		}
	}
	if (p->data == NULL) {
		log_error("%s: NULL data\n", __func__);
		return 1;
	}
	if (p->off + 1 >= p->cap) {
		log_error("%s: cap exceeded in p\n", __func__);
		return 1;
	}
	p->data[p->off] = x;
	p->data[++p->off] = NULL;
	return 0;
}