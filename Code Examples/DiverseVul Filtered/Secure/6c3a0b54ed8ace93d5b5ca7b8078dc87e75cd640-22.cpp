int edns_opt_list_remove(struct edns_option** list, uint16_t code)
{
	/* The list should already be allocated in a region. Freeing the
	 * allocated space in a region is not possible. We just unlink the
	 * required elements and they will be freed together with the region. */

	struct edns_option* prev;
	struct edns_option* curr;
	if(!list || !(*list)) return 0;

	/* Unlink and repoint if the element(s) are first in list */
	while(list && *list && (*list)->opt_code == code) {
		*list = (*list)->next;
	}

	if(!list || !(*list)) return 1;
	/* Unlink elements and reattach the list */
	prev = *list;
	curr = (*list)->next;
	while(curr != NULL) {
		if(curr->opt_code == code) {
			prev->next = curr->next;
			curr = curr->next;
		} else {
			prev = curr;
			curr = curr->next;
		}
	}
	return 1;
}