int edns_opt_list_compare(struct edns_option* p, struct edns_option* q)
{
	int r;
	while(p && q) {
		r = edns_opt_compare(p, q);
		if(r != 0)
			return r;
		p = p->next;
		q = q->next;
	}
	if(p || q) {
		/* uneven length lists */
		if(p) return 1;
		if(q) return -1;
	}
	return 0;
}