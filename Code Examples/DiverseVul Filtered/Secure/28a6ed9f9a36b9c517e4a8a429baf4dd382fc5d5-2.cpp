SPL_METHOD(SplDoublyLinkedList, setIteratorMode)
{
	zend_long value;
	spl_dllist_object *intern;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "l", &value) == FAILURE) {
		return;
	}

	intern = Z_SPLDLLIST_P(getThis());

	if (intern->flags & SPL_DLLIST_IT_FIX
		&& (intern->flags & SPL_DLLIST_IT_LIFO) != (value & SPL_DLLIST_IT_LIFO)) {
		zend_throw_exception(spl_ce_RuntimeException, "Iterators' LIFO/FIFO modes for SplStack/SplQueue objects are frozen", 0);
		return;
	}

	intern->flags = value & SPL_DLLIST_IT_MASK;

	RETURN_LONG(intern->flags);
}