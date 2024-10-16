int edns_opt_append(struct edns_data* edns, struct regional* region,
	uint16_t code, size_t len, uint8_t* data)
{
	struct edns_option** prevp;
	struct edns_option* opt;

	/* allocate new element */
	opt = (struct edns_option*)regional_alloc(region, sizeof(*opt));
	if(!opt)
		return 0;
	opt->next = NULL;
	opt->opt_code = code;
	opt->opt_len = len;
	opt->opt_data = NULL;
	if(len > 0) {
		opt->opt_data = regional_alloc_init(region, data, len);
		if(!opt->opt_data)
			return 0;
	}
	
	/* append at end of list */
	prevp = &edns->opt_list;
	while(*prevp != NULL)
		prevp = &((*prevp)->next);
	*prevp = opt;
	return 1;
}