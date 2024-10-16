struct edns_option* edns_opt_list_find(struct edns_option* list, uint16_t code)
{
	struct edns_option* p;
	for(p=list; p; p=p->next) {
		if(p->opt_code == code)
			return p;
	}
	return NULL;
}