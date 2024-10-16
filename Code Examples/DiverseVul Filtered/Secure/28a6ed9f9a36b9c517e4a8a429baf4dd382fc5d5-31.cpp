static void spl_dllist_it_helper_move_forward(spl_ptr_llist_element **traverse_pointer_ptr, int *traverse_position_ptr, spl_ptr_llist *llist, int flags) /* {{{ */
{
	if (*traverse_pointer_ptr) {
		spl_ptr_llist_element *old = *traverse_pointer_ptr;

		if (flags & SPL_DLLIST_IT_LIFO) {
			*traverse_pointer_ptr = old->prev;
			(*traverse_position_ptr)--;

			if (flags & SPL_DLLIST_IT_DELETE) {
				zval prev;
				spl_ptr_llist_pop(llist, &prev);

				zval_ptr_dtor(&prev);
			}
		} else {
			*traverse_pointer_ptr = old->next;

			if (flags & SPL_DLLIST_IT_DELETE) {
				zval prev;
				spl_ptr_llist_shift(llist, &prev);

				zval_ptr_dtor(&prev);
			} else {
				(*traverse_position_ptr)++;
			}
		}

		SPL_LLIST_DELREF(old);
		SPL_LLIST_CHECK_ADDREF(*traverse_pointer_ptr);
	}
}