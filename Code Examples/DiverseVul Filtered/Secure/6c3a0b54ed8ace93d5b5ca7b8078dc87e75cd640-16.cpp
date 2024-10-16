void edns_opt_list_free(struct edns_option* list)
{
	struct edns_option* n;
	while(list) {
		free(list->opt_data);
		n = list->next;
		free(list);
		list = n;
	}
}