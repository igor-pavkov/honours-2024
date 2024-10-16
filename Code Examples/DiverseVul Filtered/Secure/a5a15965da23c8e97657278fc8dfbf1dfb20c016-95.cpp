static zval *spl_filesystem_dir_it_current_data(zend_object_iterator *iter)
{
	spl_filesystem_iterator *iterator = (spl_filesystem_iterator *)iter;

	return &iterator->current;
}