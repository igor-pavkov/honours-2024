SPL_METHOD(SplDoublyLinkedList, offsetExists)
{
	zval              *zindex;
	spl_dllist_object *intern;
	zend_long               index;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "z", &zindex) == FAILURE) {
		return;
	}

	intern = Z_SPLDLLIST_P(getThis());
	index  = spl_offset_convert_to_long(zindex);

	RETURN_BOOL(index >= 0 && index < intern->llist->count);
} /* }}} */