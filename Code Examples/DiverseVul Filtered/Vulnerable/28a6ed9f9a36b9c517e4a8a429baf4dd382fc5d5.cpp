SPL_METHOD(SplDoublyLinkedList, offsetSet)
{
	zval                  *zindex, *value;
	spl_dllist_object     *intern;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "zz", &zindex, &value) == FAILURE) {
		return;
	}

	intern = Z_SPLDLLIST_P(getThis());

	if (Z_TYPE_P(zindex) == IS_NULL) {
		/* $obj[] = ... */
		spl_ptr_llist_push(intern->llist, value);
	} else {
		/* $obj[$foo] = ... */
		zend_long                   index;
		spl_ptr_llist_element *element;

		index = spl_offset_convert_to_long(zindex);

		if (index < 0 || index >= intern->llist->count) {
			zval_ptr_dtor(value);
			zend_throw_exception(spl_ce_OutOfRangeException, "Offset invalid or out of range", 0);
			return;
		}

		element = spl_ptr_llist_offset(intern->llist, index, intern->flags & SPL_DLLIST_IT_LIFO);

		if (element != NULL) {
			/* call dtor on the old element as in spl_ptr_llist_pop */
			if (intern->llist->dtor) {
				intern->llist->dtor(element);
			}

			/* the element is replaced, delref the old one as in
			 * SplDoublyLinkedList::pop() */
			zval_ptr_dtor(&element->data);
			ZVAL_COPY_VALUE(&element->data, value);

			/* new element, call ctor as in spl_ptr_llist_push */
			if (intern->llist->ctor) {
				intern->llist->ctor(element);
			}
		} else {
			zval_ptr_dtor(value);
			zend_throw_exception(spl_ce_OutOfRangeException, "Offset invalid", 0);
			return;
		}
	}
} /* }}} */