static zval *spl_ptr_llist_last(spl_ptr_llist *llist) /* {{{ */
{
	spl_ptr_llist_element *tail = llist->tail;

	if (tail == NULL) {
		return NULL;
	} else {
		return &tail->data;
	}
}