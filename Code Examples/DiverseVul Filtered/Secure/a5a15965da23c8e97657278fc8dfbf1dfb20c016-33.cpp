static void spl_filesystem_tree_it_dtor(zend_object_iterator *iter)
{
	spl_filesystem_iterator *iterator = (spl_filesystem_iterator *)iter;

	if (!Z_ISUNDEF(iterator->intern.data)) {
		zval *object = &iterator->intern.data;
		zval_ptr_dtor(object);
	} else {
		if (!Z_ISUNDEF(iterator->current)) {
			zval_ptr_dtor(&iterator->current);
			ZVAL_UNDEF(&iterator->current);
		}
	}
}