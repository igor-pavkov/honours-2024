struct edns_option* edns_opt_copy_alloc(struct edns_option* list)
{
	struct edns_option* result = NULL, *cur = NULL, *s;
	while(list) {
		/* copy edns option structure */
		s = memdup(list, sizeof(*list));
		if(!s) {
			edns_opt_list_free(result);
			return NULL;
		}
		s->next = NULL;

		/* copy option data */
		if(s->opt_data) {
			s->opt_data = memdup(s->opt_data, s->opt_len);
			if(!s->opt_data) {
				free(s);
				edns_opt_list_free(result);
				return NULL;
			}
		}

		/* link into list */
		if(cur)
			cur->next = s;
		else	result = s;
		cur = s;

		/* examine next element */
		list = list->next;
	}
	return result;
}